//#pragma execution_character_set("utf-8")
#include "CUGGIS.h"
#include "Select.h"
//ͼ�������
#include <qgslayertreemapcanvasbridge.h> 
#include <qgslayertreeview.h>
#include <qgslayertreemodel.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgslayertree.h>
#include <QMouseEvent>
#include <QMenu>
#include <QgsFeatureRequest.h>
#include <QgsVectorLayerSelectionManager.h>
#include <qgsmaptool.h> // ���ڵ�ͼ����
#include <qgsgeometry.h>
#include <QCursor.h>
#include <qgsfeatureiterator.h>
#include <qmessagebox.h>
#include <qgsgeometry.h>       // ���ڼ��β���
#include <qgsvectorlayer.h>    // ����ʸ��ͼ��
#include <qgsfields.h>         // �����ֶβ���
#include <qgsvectorlayerselectionmanager.h> // ����ѡ�������
#include <qgsfeature.h>
#include <qgsmapmouseevent.h>
#include <qgssourcefieldsproperties.h>

//���תshp
#include "readExcel.h"

void CUGGIS::on_actionOpenAttrTable_triggered()
{
    openEditableAttributeTable();
}
void CUGGIS::openEditableAttributeTable()
{
    if (!m_curMapLayer || m_curMapLayer->type() != QgsMapLayerType::VectorLayer) {
        QMessageBox::warning(this, "����", "��ѡ����Ч��ʸ��ͼ��");
        return;
    }

    QgsVectorLayer* vectorLayer = static_cast<QgsVectorLayer*>(m_curMapLayer);

    QDialog dialog(this);
    dialog.setWindowTitle("���Ա�");

    // ���öԻ���Ĵ�С
    dialog.resize(1000, 1000);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTableWidget* tableWidget = new QTableWidget(&dialog);

    const QgsFields& fields = vectorLayer->fields();
    tableWidget->setColumnCount(fields.count());
    QStringList headers;
    for (int i = 0; i < fields.count(); ++i) {
        headers << fields.at(i).name();
    }
    tableWidget->setHorizontalHeaderLabels(headers);

    QgsFeature feature;
    int row = 0;
    tableWidget->setRowCount(vectorLayer->featureCount());
    QgsFeatureIterator iterator = vectorLayer->getFeatures();
    while (iterator.nextFeature(feature)) {
        for (int col = 0; col < fields.count(); ++col) {
            QTableWidgetItem* item = new QTableWidgetItem(feature.attribute(col).toString());
            tableWidget->setItem(row, col, item);
        }
        ++row;
    }

    layout->addWidget(tableWidget);


    // �������һ�а�ť
    QPushButton* addRowButton = new QPushButton("+", &dialog);
    layout->addWidget(addRowButton);
    connect(addRowButton, &QPushButton::clicked, [&]() {
        int newRow = tableWidget->rowCount();
        tableWidget->insertRow(newRow);
        for (int col = 0; col < fields.count(); ++col) {
            QTableWidgetItem* item = new QTableWidgetItem("");
            tableWidget->setItem(newRow, col, item);
        }
        });

    // ���ɾ��һ�а�ť
    QPushButton* deleteRowButton = new QPushButton("-", &dialog);
    layout->addWidget(deleteRowButton);
    connect(deleteRowButton, &QPushButton::clicked, [&]() {
        int currentRow = tableWidget->currentRow();
        if (currentRow >= 0) {
            tableWidget->removeRow(currentRow);
        }
        });

    // ��ӱ��水ť
    QPushButton* saveButton = new QPushButton("����", &dialog);
    layout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, [&]() {
        vectorLayer->startEditing();
        for (int i = 0; i < tableWidget->rowCount(); ++i) {
            QgsFeature feature = vectorLayer->getFeature(i);
            for (int j = 0; j < tableWidget->columnCount(); ++j) {
                QString newValue = tableWidget->item(i, j)->text();
                feature.setAttribute(j, newValue);
            }
            vectorLayer->updateFeature(feature);
        }
        vectorLayer->commitChanges();
        dialog.accept();
        QMessageBox::information(this, "�ɹ�", "���Ա��ѱ���");
        });

    dialog.exec();
}

void CUGGIS::on_actionChoose_triggered()
{
    CustomIdentifyTool* tool = new CustomIdentifyTool(m_mapCanvas);
    m_mapCanvas->setMapTool(tool);
}

void CUGGIS::on_actionFinish_triggered()
{
    // ��鵱ǰ�Ƿ���ʸ��ͼ��
    if (m_curMapLayer && m_curMapLayer->type() == QgsMapLayerType::VectorLayer) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(m_curMapLayer);

        if (vectorLayer) {
            // ���ͼ���ϵ�����ѡ��Ҫ��
            vectorLayer->removeSelection();
            m_mapCanvas->refresh();
        }
    }

    // ֹͣѡ��������л���Ĭ�Ϲ��ߣ�
    m_mapCanvas->setMapTool(nullptr);
}

void CUGGIS::on_actionOpenFields_triggered()
{
    QgsVectorLayer* layer = (QgsVectorLayer*)m_curMapLayer;
    if (layer == nullptr || !layer->isValid() || QgsMapLayerType::VectorLayer != m_curMapLayer->type())
    {
        return;
    }
    layer->startEditing();
    //��ʾ���Խṹ�ֶ�
    QgsSourceFieldsProperties* pWidget = new QgsSourceFieldsProperties(layer, nullptr);
    pWidget->loadRows();
    pWidget->show();
}

void CUGGIS::on_actionExcelShp_triggered()
{
    readExcel* r = new readExcel(m_mapCanvas);  //����һ���½���
    r->show();
    m_mapCanvas->refresh();
}
void  CUGGIS::on_actionUndo_triggered()
{
    if (m_AddPoint) {
        m_AddPoint->undo();
    }
    else {
        QMessageBox::warning(this, "Error", "PolygonEditTool is not active.");
    }
}
void  CUGGIS::on_actionRedo_triggered()
{
    if (m_AddPoint) {
        m_AddPoint->redo();
    }
    else {
        QMessageBox::warning(this, "Error", "PolygonEditTool is not active.");
    }
}