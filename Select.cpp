
#include "Select.h"
#include <qgsproject.h>
#include <qgsvectorlayer.h>
#include <qgsgeometry.h>
#include <QDebug>
#include <qgsmapmouseevent.h>
#include <QMessageBox>

// 构造函数
CustomIdentifyTool::CustomIdentifyTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), m_contextMenu(new QMenu()) {
    // 添加 "修改要素属性" 到上下文菜单
    QAction* action = m_contextMenu->addAction("Edit Feature Attributes");

    // 使用 lambda 表达式连接信号和槽
    connect(action, &QAction::triggered, this, [this]() {
        this->showAttributeEditor();
        });
}

// 获取第一个矢量图层
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

// 鼠标释放事件处理
void CustomIdentifyTool::canvasReleaseEvent(QgsMapMouseEvent* event) {
    m_currentLayer = getFirstVectorLayer();
    if (!m_currentLayer) {
        qDebug() << "No vector layers found!";
        return;
    }

    // 获取鼠标点击的屏幕坐标
    int x = event->pos().x();
    int y = event->pos().y();

    // 使用 identify 方法进行空间查询
    QList<QgsMapToolIdentify::IdentifyResult> results = identify(
        x, y,
        QList<QgsMapLayer*>{m_currentLayer},
        QgsMapToolIdentify::TopDownStopAtFirst
    );

    if (!results.isEmpty()) {
        // 获取第一个匹配的要素
        m_currentFeature = results.first().mFeature;

        // 仅在右键点击时显示菜单
        if (event->button() == Qt::RightButton) {
            m_contextMenu->exec(event->globalPos());
        }
        else {
            // 左键的行为，高亮显示要素
            m_currentLayer->select(m_currentFeature.id());
        }
    }
    else {
        qDebug() << "No feature selected!";
    }
}

// 显示属性编辑器
void CustomIdentifyTool::showAttributeEditor() {
    if (!m_currentLayer || !m_currentFeature.isValid()) return;

    // 创建对话框
    QDialog dialog;
    dialog.setWindowTitle("Edit Feature Attributes");
    // 设置对话框的大小
    dialog.resize(1500, 300);
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTableWidget* tableWidget = new QTableWidget(&dialog);
    const QgsFields& fields = m_currentLayer->fields(); // 使用m_currentLayer
    tableWidget->setColumnCount(fields.count());

    QStringList headers;
    for (int i = 0; i < fields.count(); ++i) {
        headers << fields.at(i).name();
    }

    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setRowCount(1);

    // 将当前要素的属性填入表格
    for (int col = 0; col < fields.count(); ++col) {
        QTableWidgetItem* item = new QTableWidgetItem(m_currentFeature.attribute(col).toString());
        tableWidget->setItem(0, col, item);
    }

    layout->addWidget(tableWidget);

    // 保存按钮
    QPushButton* saveButton = new QPushButton("Save", &dialog);
    layout->addWidget(saveButton);

    // 连接保存按钮的点击信号
    connect(saveButton, &QPushButton::clicked, [&]() {
        if (!m_currentLayer->isEditable()) {
            if (!m_currentLayer->startEditing()) {
                QMessageBox::warning(&dialog, "Error", "Cannot edit this layer");
                return;
            }
        }

        for (int col = 0; col < fields.count(); ++col) {
            QString newValue = tableWidget->item(0, col)->text();
            m_currentFeature.setAttribute(col, newValue); // 更新当前要素的属性
        }

        if (!m_currentLayer->updateFeature(m_currentFeature)) {
            QMessageBox::warning(&dialog, "Error", "Cannot update feature");
            return;
        }

        // 提交更改前确保所有操作成功
        if (!m_currentLayer->commitChanges()) {
            QMessageBox::warning(&dialog, "Error", "Cannot Submit Changes, Please Check If The Layer Is Locked Or Read-Only");
            return;
        }

        // 确保图层刷新，显示更新后的属性
        m_currentLayer->triggerRepaint();

        dialog.accept();
        QMessageBox::information(&dialog, "Success", "Attributes Updated Successfully");
        });

    dialog.exec(); // 执行对话框
}
