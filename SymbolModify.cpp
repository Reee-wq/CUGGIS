#include "SymbolModify.h"
#include<QMessageBox>
#include<QgsVectorLayer.h>
#include <qgsrenderer.h>
#include <qgssymbol.h>
#include <qgsmapcanvas.h>
#include<qgssinglesymbolrenderer.h>



SymbolModify::SymbolModify(QWidget* parent, QgsMapLayer* layer)
	: QMainWindow(parent), m_curMapLayer(layer)  // 在构造函数中初始化 m_curMapLayer
{
	ui.setupUi(this);
	QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(m_curMapLayer);
	if(vectorLayer){
	QgsWkbTypes::GeometryType geomType = vectorLayer->geometryType();
    mStyle = StyleManager::getStyle(); //获取样式
    // 确保 mModel 被初始化
    mModel = new QgsStyleProxyModel(mStyle);

    // 配置图标大小和网格大小 
    double iconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().maxWidth() * 10; // 图标大小
    ui.listView->setIconSize(QSize(static_cast<int>(iconSize * 0.3), static_cast<int>(iconSize * 0.3)));
    ui.listView->setGridSize(QSize(static_cast<int>(iconSize * 0.35), static_cast<int>(iconSize * 0.45)));
    mModel->addDesiredIconSize(ui.listView->iconSize()); // 设置图标大小

    // 设置模型到视图
    ui.listView->setModel(mModel);
    ui.listView->setSelectionBehavior(QAbstractItemView::SelectItems);  // 单选
    ui.listView->setSelectionMode(QAbstractItemView::ExtendedSelection);  // 多选
    ui.listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 不允许编辑
    ui.listView->setWordWrap(true);  // 自动换行

    // 禁用符号类型过滤器
    mModel->setSymbolTypeFilterEnabled(false);  // 禁用符号类型过滤器

    // 设置符号类型
    QVector<QgsSymbol::SymbolType> allSymbolTypes = {
        QgsSymbol::SymbolType::Marker,
        QgsSymbol::SymbolType::Line,
        QgsSymbol::SymbolType::Fill
    };
    int type = 0;
    if (geomType == QgsWkbTypes::PointGeometry) {
        type = 0;
    }
    else if (geomType == QgsWkbTypes::LineGeometry) {
        type = 1;
    }
    else if (geomType == QgsWkbTypes::PolygonGeometry) {
        type = 2;
    }
    mModel->setEntityFilterEnabled(true); // 符号类型过滤器
    mModel->setSymbolTypeFilterEnabled(true); // 符号类型过滤器
    mModel->setSymbolType(static_cast<QgsSymbol::SymbolType>(type)); // 设置符号类型
	}

    connect(ui.buttonModify, &QPushButton::clicked, this, &SymbolModify::on_buttonModify_click);

}

SymbolModify::~SymbolModify()
{}

void SymbolModify::on_buttonModify_click() {
    if (m_curMapLayer == nullptr)
        return;
    if (QgsMapLayerType::VectorLayer != m_curMapLayer->type())
        return;

    // 获取当前选中的符号项
    QModelIndexList selectedIndexes = ui.listView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "没有选择符号", "请选择一个符号进行修改！");
        return;
    }
    // 获取第一个选中的索引
    QModelIndex index = selectedIndexes.first();
    QVariant data = mModel->data(index);
    QString symbolName = data.toString();

    // 获取符号对象
    QgsSymbol* symbol = mStyle->symbol(symbolName);

    QgsVectorLayer* vecLayer = qobject_cast<QgsVectorLayer*>(m_curMapLayer);
    QgsWkbTypes::GeometryType geomType = vecLayer->geometryType(); // 获取几何类型
    if (vecLayer) {
        // 创建符号渲染器
        QgsSingleSymbolRenderer* symRenderer = nullptr;

        if (geomType == QgsWkbTypes::PointGeometry) {
            QgsMarkerSymbol* sym = dynamic_cast<QgsMarkerSymbol*>(symbol);
            if (sym) {
                symRenderer = new QgsSingleSymbolRenderer(sym);
            }
        }
        else if (geomType == QgsWkbTypes::LineGeometry) {
            QgsLineSymbol* sym = dynamic_cast<QgsLineSymbol*>(symbol);
            if (sym) {
                symRenderer = new QgsSingleSymbolRenderer(sym);
            }
        }
        else if (geomType == QgsWkbTypes::PolygonGeometry) {
            QgsFillSymbol* sym = dynamic_cast<QgsFillSymbol*>(symbol);
            if (sym) {
                symRenderer = new QgsSingleSymbolRenderer(sym);
            }
        }

        if (symRenderer) {
            // 设置渲染器到图层
            vecLayer->setRenderer(symRenderer);
            // 刷新图层
            vecLayer->triggerRepaint();
            // 关闭对话框
            close();
        }

    }

}