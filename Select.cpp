
#include "Select.h"
#include <qgsproject.h>
#include <qgsvectorlayer.h>
#include <qgsgeometry.h>
#include <QDebug>
#include <qgsmapmouseevent.h>
#include <QMessageBox>

// ���캯��
CustomIdentifyTool::CustomIdentifyTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), m_contextMenu(new QMenu()) {
    // ��� "�޸�Ҫ������" �������Ĳ˵�
    QAction* action = m_contextMenu->addAction("Edit Feature Attributes");

    // ʹ�� lambda ���ʽ�����źźͲ�
    connect(action, &QAction::triggered, this, [this]() {
        this->showAttributeEditor();
        });
}

// ��ȡ��һ��ʸ��ͼ��
QgsVectorLayer* CustomIdentifyTool::getFirstVectorLayer() {
    auto layers = QgsProject::instance()->mapLayers().values();
    for (auto layer : layers) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer;
        }
    }
    return nullptr;
}

// ����ͷ��¼�����
void CustomIdentifyTool::canvasReleaseEvent(QgsMapMouseEvent* event) {
    m_currentLayer = getFirstVectorLayer();
    if (!m_currentLayer) {
        qDebug() << "No vector layers found!";
        return;
    }

    // ��ȡ���������Ļ����
    int x = event->pos().x();
    int y = event->pos().y();

    // ʹ�� identify �������пռ��ѯ
    QList<QgsMapToolIdentify::IdentifyResult> results = identify(
        x, y,
        QList<QgsMapLayer*>{m_currentLayer},
        QgsMapToolIdentify::TopDownStopAtFirst
    );

    if (!results.isEmpty()) {
        // ��ȡ��һ��ƥ���Ҫ��
        m_currentFeature = results.first().mFeature;

        // �����Ҽ����ʱ��ʾ�˵�
        if (event->button() == Qt::RightButton) {
            m_contextMenu->exec(event->globalPos());
        }
        else {
            // �������Ϊ��������ʾҪ��
            m_currentLayer->select(m_currentFeature.id());
        }
    }
    else {
        qDebug() << "No feature selected!";
    }
}

// ��ʾ���Ա༭��
void CustomIdentifyTool::showAttributeEditor() {
    if (!m_currentLayer || !m_currentFeature.isValid()) return;

    // �����Ի���
    QDialog dialog;
    dialog.setWindowTitle("Edit Feature Attributes");
    // ���öԻ���Ĵ�С
    dialog.resize(1500, 300);
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTableWidget* tableWidget = new QTableWidget(&dialog);
    const QgsFields& fields = m_currentLayer->fields(); // ʹ��m_currentLayer
    tableWidget->setColumnCount(fields.count());

    QStringList headers;
    for (int i = 0; i < fields.count(); ++i) {
        headers << fields.at(i).name();
    }

    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setRowCount(1);

    // ����ǰҪ�ص�����������
    for (int col = 0; col < fields.count(); ++col) {
        QTableWidgetItem* item = new QTableWidgetItem(m_currentFeature.attribute(col).toString());
        tableWidget->setItem(0, col, item);
    }

    layout->addWidget(tableWidget);

    // ���水ť
    QPushButton* saveButton = new QPushButton("Save", &dialog);
    layout->addWidget(saveButton);

    // ���ӱ��水ť�ĵ���ź�
    connect(saveButton, &QPushButton::clicked, [&]() {
        if (!m_currentLayer->isEditable()) {
            if (!m_currentLayer->startEditing()) {
                QMessageBox::warning(&dialog, "Error", "Cannot edit this layer");
                return;
            }
        }

        for (int col = 0; col < fields.count(); ++col) {
            QString newValue = tableWidget->item(0, col)->text();
            m_currentFeature.setAttribute(col, newValue); // ���µ�ǰҪ�ص�����
        }

        if (!m_currentLayer->updateFeature(m_currentFeature)) {
            QMessageBox::warning(&dialog, "Error", "Cannot update feature");
            return;
        }

        // �ύ����ǰȷ�����в����ɹ�
        if (!m_currentLayer->commitChanges()) {
            QMessageBox::warning(&dialog, "Error", "Cannot Submit Changes, Please Check If The Layer Is Locked Or Read-Only");
            return;
        }

        // ȷ��ͼ��ˢ�£���ʾ���º������
        m_currentLayer->triggerRepaint();

        dialog.accept();
        QMessageBox::information(&dialog, "Success", "Attributes Updated Successfully");
        });

    dialog.exec(); // ִ�жԻ���
}
