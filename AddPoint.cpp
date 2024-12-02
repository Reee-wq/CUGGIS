#include "AddPoint.h"
#include <QgsFeature.h>
#include <QgsGeometry.h>
#include <QgsPointXY.h>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <qgsmapmouseevent.h>

AddPoint::AddPoint(QgsMapCanvas* canvas, QgsVectorLayer* layer, QObject* parent)
    : QgsMapTool(canvas), m_mapCanvas(canvas), m_layer(layer)
{
    // ��ʼͼ��༭ģʽ
    if (!m_layer->isEditable())
    {
        m_layer->startEditing();  // �����δ�༭���������༭ģʽ
    }
  
}

AddPoint::~AddPoint() {}

void AddPoint::canvasPressEvent(QgsMapMouseEvent* e)
{

    // ȷ��ͼ����Ч���ҿɱ༭
    if (!m_layer || !m_layer->isEditable() || m_layer->geometryType() != QgsWkbTypes::PointGeometry) {
        QMessageBox::warning(nullptr, "Invalid Layer", "Please select a valid and editable point layer!");
        return;
    }

    // ��ȡ���λ�õĵ�ͼ����
    QgsPointXY mapPoint = e->mapPoint();

    // ��ȡ��ǰͼ����ֶ�����
    QStringList fieldNames;
    for (int i = 0; i < m_layer->fields().count(); ++i) {
        fieldNames.append(m_layer->fields().field(i).name());
    }

    // ������������Ի���
    QStringList attributes = showAttributeInputDialog(fieldNames);

    if (!attributes.isEmpty()) {
        // �����㼸����
        QgsGeometry pointGeom = QgsGeometry::fromPointXY(mapPoint);

        // ����һ���µ�Ҫ�أ�Feature��
        QgsFeature feature;
        feature.setGeometry(pointGeom);

        // ��������
        for (int i = 0; i < fieldNames.count(); ++i) {
            QString fieldName = fieldNames[i];
            QString fieldValue = attributes[i];

            // �������������ֵ����
            feature.setAttribute(fieldName, fieldValue.isEmpty() ? QVariant() : fieldValue);
        }

        // ��ͼ�������Ҫ��
        QgsFeatureList features;
        features.append(feature);

        if (!m_layer->dataProvider()->addFeatures(features)) {
            QMessageBox::warning(nullptr, "error", "Failed to add features, please check if the layer supports editing!");
            return;
        }
        // �ύ���ĵ�������ջ
        m_layer->commitChanges();
        // ˢ��ͼ��
        m_layer->triggerRepaint();
        m_mapCanvas->refresh();
    }
    else {
        QMessageBox::information(nullptr, "The operation is canceled", "You've canceled the add point!");
    }
}



QStringList AddPoint::showAttributeInputDialog(const QStringList& fieldNames)
{
    // �����Ի���
    QDialog dialog;
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // �ֶ������
    QMap<QString, QLineEdit*> fieldInputs;
    for (const QString& fieldName : fieldNames) {
        QLabel* label = new QLabel(fieldName + ": ");
        QLineEdit* lineEdit = new QLineEdit(&dialog);

        // �����ֶ�����������ʾ
        QVariant::Type fieldType = m_layer->fields().field(fieldName).type();
        if (fieldType == QVariant::Int) {
            lineEdit->setPlaceholderText("Please enter an integer");
        }
        else if (fieldType == QVariant::Double) {
            lineEdit->setPlaceholderText("Please enter a float");
        }
        else if (fieldType == QVariant::String) {
            lineEdit->setPlaceholderText("Please enter txt");
        }

        fieldInputs[fieldName] = lineEdit;
        layout->addWidget(label);
        layout->addWidget(lineEdit);
    }

    // ��� OK �� Cancel ��ť
    QPushButton* okButton = new QPushButton("OK", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    // ���Ӱ�ť�ۺ���
    QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // ��ʾ�Ի��򲢵ȴ��û�����
    if (dialog.exec() == QDialog::Accepted) {
        QStringList attributes;
        for (const QString& fieldName : fieldNames) {
            QLineEdit* lineEdit = fieldInputs[fieldName];
            attributes.append(lineEdit->text());
        }
        return attributes;  // ��������ֵ�б�
    }
    return QStringList();  // �û�ȡ����
}


void AddPoint::undo()
{
    if (m_layer && m_layer->isEditable()) {
        if (m_layer->undoStack()->canUndo()) {
            m_layer->undoStack()->undo();
            m_layer->triggerRepaint();
            m_mapCanvas->refresh();
        }
        else {
            QMessageBox::information(nullptr, "Info", "No actions to undo.");
        }
    }
}

void AddPoint::redo()
{
    if (m_layer && m_layer->isEditable()) {
        if (m_layer->undoStack()->canRedo()) {
            m_layer->undoStack()->redo();
            m_layer->triggerRepaint();
            m_mapCanvas->refresh();
        }
        else {
            QMessageBox::information(nullptr, "Info", "No actions to redo.");
        }
    }
}