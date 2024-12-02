//#pragma execution_character_set("utf-8")
#include "CUGGIS.h"
#include "Select.h"
//图层管理器
#include <qgslayertreemapcanvasbridge.h> 
#include <qgslayertreeview.h>
#include <qgslayertreemodel.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgslayertree.h>
#include <QMouseEvent>
#include <QMenu>
#include <QgsFeatureRequest.h>
#include <QgsVectorLayerSelectionManager.h>
#include <qgsmaptool.h> // 用于地图工具
#include <qgsgeometry.h>
#include <QCursor.h>
#include <qgsfeatureiterator.h>
#include <qmessagebox.h>
#include <qgsgeometry.h>       // 用于几何操作
#include <qgsvectorlayer.h>    // 用于矢量图层
#include <qgsfields.h>         // 用于字段操作
#include <qgsvectorlayerselectionmanager.h> // 用于选择管理器
#include <qgsfeature.h>
#include <qgsmapmouseevent.h>
#include <qgssourcefieldsproperties.h>

//表格转shp
#include "readExcel.h"

void CUGGIS::on_actionOpenAttrTable_triggered()
{
    openEditableAttributeTable();
}
void CUGGIS::openEditableAttributeTable()
{
    if (!m_curMapLayer || m_curMapLayer->type() != QgsMapLayerType::VectorLayer) {
        QMessageBox::warning(this, "警告", "请选择有效的矢量图层");
        return;
    }

    QgsVectorLayer* vectorLayer = static_cast<QgsVectorLayer*>(m_curMapLayer);

    QDialog dialog(this);
    dialog.setWindowTitle("属性表");

    // 设置对话框的大小
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


    // 添加新增一行按钮
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

    // 添加删除一行按钮
    QPushButton* deleteRowButton = new QPushButton("-", &dialog);
    layout->addWidget(deleteRowButton);
    connect(deleteRowButton, &QPushButton::clicked, [&]() {
        int currentRow = tableWidget->currentRow();
        if (currentRow >= 0) {
            tableWidget->removeRow(currentRow);
        }
        });

    // 添加保存按钮
    QPushButton* saveButton = new QPushButton("保存", &dialog);
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
        QMessageBox::information(this, "成功", "属性表已保存");
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
    // 检查当前是否有矢量图层
    if (m_curMapLayer && m_curMapLayer->type() == QgsMapLayerType::VectorLayer) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(m_curMapLayer);

        if (vectorLayer) {
            // 清除图层上的所有选中要素
            vectorLayer->removeSelection();
            m_mapCanvas->refresh();
        }
    }

    // 停止选择操作（切换回默认工具）
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
    //显示属性结构字段
    QgsSourceFieldsProperties* pWidget = new QgsSourceFieldsProperties(layer, nullptr);
    pWidget->loadRows();
    pWidget->show();
}

void CUGGIS::on_actionExcelShp_triggered()
{
    readExcel* r = new readExcel(m_mapCanvas);  //创建一个新界面
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