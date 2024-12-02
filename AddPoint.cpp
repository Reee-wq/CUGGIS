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
    // 开始图层编辑模式
    if (!m_layer->isEditable())
    {
        m_layer->startEditing();  // 如果尚未编辑，则启动编辑模式
    }
  
}

AddPoint::~AddPoint() {}

void AddPoint::canvasPressEvent(QgsMapMouseEvent* e)
{

    // 确保图层有效并且可编辑
    if (!m_layer || !m_layer->isEditable() || m_layer->geometryType() != QgsWkbTypes::PointGeometry) {
        QMessageBox::warning(nullptr, "Invalid Layer", "Please select a valid and editable point layer!");
        return;
    }

    // 获取点击位置的地图坐标
    QgsPointXY mapPoint = e->mapPoint();

    // 获取当前图层的字段名称
    QStringList fieldNames;
    for (int i = 0; i < m_layer->fields().count(); ++i) {
        fieldNames.append(m_layer->fields().field(i).name());
    }

    // 弹出属性输入对话框
    QStringList attributes = showAttributeInputDialog(fieldNames);

    if (!attributes.isEmpty()) {
        // 创建点几何体
        QgsGeometry pointGeom = QgsGeometry::fromPointXY(mapPoint);

        // 创建一个新的要素（Feature）
        QgsFeature feature;
        feature.setGeometry(pointGeom);

        // 设置属性
        for (int i = 0; i < fieldNames.count(); ++i) {
            QString fieldName = fieldNames[i];
            QString fieldValue = attributes[i];

            // 根据输入的属性值设置
            feature.setAttribute(fieldName, fieldValue.isEmpty() ? QVariant() : fieldValue);
        }

        // 向图层中添加要素
        QgsFeatureList features;
        features.append(feature);

        if (!m_layer->dataProvider()->addFeatures(features)) {
            QMessageBox::warning(nullptr, "error", "Failed to add features, please check if the layer supports editing!");
            return;
        }
        // 提交更改到撤销堆栈
        m_layer->commitChanges();
        // 刷新图层
        m_layer->triggerRepaint();
        m_mapCanvas->refresh();
    }
    else {
        QMessageBox::information(nullptr, "The operation is canceled", "You've canceled the add point!");
    }
}



QStringList AddPoint::showAttributeInputDialog(const QStringList& fieldNames)
{
    // 创建对话框
    QDialog dialog;
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // 字段输入框
    QMap<QString, QLineEdit*> fieldInputs;
    for (const QString& fieldName : fieldNames) {
        QLabel* label = new QLabel(fieldName + ": ");
        QLineEdit* lineEdit = new QLineEdit(&dialog);

        // 根据字段类型设置提示
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

    // 添加 OK 和 Cancel 按钮
    QPushButton* okButton = new QPushButton("OK", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    // 连接按钮槽函数
    QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // 显示对话框并等待用户输入
    if (dialog.exec() == QDialog::Accepted) {
        QStringList attributes;
        for (const QString& fieldName : fieldNames) {
            QLineEdit* lineEdit = fieldInputs[fieldName];
            attributes.append(lineEdit->text());
        }
        return attributes;  // 返回属性值列表
    }
    return QStringList();  // 用户取消了
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