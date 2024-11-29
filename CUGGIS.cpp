#include "CUGGIS.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include "qgsproject.h"
#include "qboxlayout.h"
#include "QgsLayerTreeViewDefaultActions.h"
#include "QtGisLayerTreeViewMenuProvider.h"
#include "qgsrasterlayer.h"
#include "qgslayertreeregistrybridge.h"
#include "qgslayertreegroup.h"

#include "qgsmaplayer.h"
#include <QTabWidget>
#include <QProgressBar>
#include <QGridLayout>
#include <QListWidget>
#include <QGroupBox>
#include "QtGisLayerTreeViewMenuProvider.h"

#include "VectorLayerPropertiesWidget.h"
#include "RasterLayerPropertiesWidget.h"
#include "qgslayerpropertieswidget.h"
#include "qgsmapcanvas.h"
#include "qgsapplication.h"
#include "qgsstatusbar.h"
#include "qgstaskmanagerwidget.h"
#include "qgsdoublespinbox.h"
#include "qgsdockwidget.h"

#include "QgsLayerTreeView.h"
#include "QgsLayerTreeModel.h"
#include "QgsLayerTreeMapCanvasBridge.h"
#include "QgsLayerTreeRegistryBridge.h"
#include "QgsLayerTree.h"
#include "QgsLayerTreeLayer.h"
#include "QgsLayerTreeViewDefaultActions.h"
#include "QgsLayerTreeUtils.h"

//#include "qgslayerpropertieswidget.h"
#include "qfilesystemmodel.h"



CUGGIS::CUGGIS(QWidget *parent)
    : QMainWindow(parent)
{
	m_mapCanvas = nullptr;
	// 图层管理器
	m_layerTreeView = nullptr;
	m_layerTreeCanvasBridge = nullptr;
    ui.setupUi(this);
    	// 初始化地图画布
	m_mapCanvas = new QgsMapCanvas();
	this->setCentralWidget(m_mapCanvas);
	// 初始化图层管理器
	//m_layerTreeView = new QgsLayerTreeView(this);
	initLayerTreeView();
	m_curMapLayer = nullptr;
	//矢量，地理处理
	//凸包
	//m_convexHull = nullptr;
	//数据处理，ID3andC4
	m_ID3andC4 = nullptr;
	connect(ui.actionQgsStylelibMng, &QAction::triggered, this, &CUGGIS::on_actionQgsStylelibMng_triggered);
	connect(ui.actionSelfStylelibMng, &QAction::triggered, this, &CUGGIS::on_actionSelfStylelibMng_triggered);
	connect(m_layerTreeView, &QgsLayerTreeView::doubleClicked, this, &CUGGIS::onLayerTreeItemDoubleClicked);
	// 创建文件系统模型
	QFileSystemModel* m_model = new QFileSystemModel(this);

	// 创建 QTreeView
	QTreeView* m_treeView = new QTreeView(this);
	m_model->setRootPath("D:/");

	// 设置文件系统模型
	m_treeView->setModel(m_model);
	m_treeView->setRootIndex(m_model->index(QDir::currentPath()));
	ui.fileTree->setWidget(m_treeView);

	
}

CUGGIS::~CUGGIS()
{}

void CUGGIS::onCurrentLayerChanged(QgsMapLayer* layer)
{
	QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
	if (vectorLayer)
	{
		//判断是否为你想要处理的图层类型，例如shapefile
		if (vectorLayer->dataProvider()->name() == "ogr" && vectorLayer->source().endsWith(".shp"))
		{
			//获取矢量图层的路径
			m_curMapLayer = vectorLayer;
		}
	}
}

void CUGGIS::on_actionAddVectorLayer_triggered()
{
	QStringList layerPathList = QFileDialog::getOpenFileNames(this, QStringLiteral("choose vectorlayer"), "", "shapefile (*.shp)");
	QList<QgsMapLayer*> layerList;
	for (const QString& layerPath : layerPathList)
	{
		QFileInfo fi(layerPath);
		if (!fi.exists()) { return; }
		QString layerBaseName = fi.baseName(); // 图层名称

		QgsVectorLayer* vecLayer = new QgsVectorLayer(layerPath, layerBaseName);
		if (!vecLayer) { return; }
		if (!vecLayer->isValid())
		{
			QMessageBox::information(0, "", "layer is invalid");
			return;
		}
		layerList << vecLayer;
	}
	if (layerList.size() < 1)
		return;
	
	//选择图层
	connect(m_layerTreeView, &QgsLayerTreeView::currentLayerChanged, this, &CUGGIS::onCurrentLayerChanged);

	m_curMapLayer = layerList[0];
	QgsProject::instance()->addMapLayers(layerList);
	// 添加图层到图层树
	//addLayersBelowLast(layerList);
	// 执行动态投影
	//setLayersCrsToLastLayerCrs();
	m_mapCanvas->refresh();
}

void CUGGIS::on_actionAddRasterLayer_triggered()
{
	QStringList layerPathList = QFileDialog::getOpenFileNames(this, QStringLiteral("选择栅格数据"), "", "Image (*.img *.tif *.tiff)");
	QList<QgsMapLayer*> layerList;
	for (const QString& layerPath : layerPathList)
	{
		QFileInfo fi(layerPath);
		if (!fi.exists()) { return; }
		QString layerBaseName = fi.baseName(); // 图层名称

		QgsRasterLayer* rasterLayer = new QgsRasterLayer(layerPath, layerBaseName);
		if (!rasterLayer) { return; }
		if (!rasterLayer->isValid())
		{
			QMessageBox::information(0, "", "layer is invalid");
			return;
		}
		layerList << rasterLayer;
	}
	m_curMapLayer = layerList[0];
	QgsProject::instance()->addMapLayers(layerList);
	m_mapCanvas->refresh();
}

void CUGGIS::on_actionLayerProperties_triggered()
{

	if (m_curMapLayer)
	{
		// 检查图层类型
		if (m_curMapLayer->type() == QgsMapLayerType::VectorLayer)
		{
			// 打开矢量图层属性管理窗口
			VectorLayerPropertiesWidget* widget = new VectorLayerPropertiesWidget(m_curMapLayer, this);
			widget->exec();
		}
		else if (m_curMapLayer->type() == QgsMapLayerType::RasterLayer)
		{
			// 打开栅格图层属性管理窗口
			RasterLayerPropertiesWidget* widget = new RasterLayerPropertiesWidget(m_curMapLayer, this);
			widget->exec();
		}
		else
		{
			qDebug() << "The active layer is not a vector or raster layer.";
		}
	}
	else
	{
		qDebug() << "No active layer found.";
	}
	
}


void CUGGIS::removeLayer()
{
	if (!m_layerTreeView) { return; }

	QModelIndexList indexes;
	while ((indexes = m_layerTreeView->selectionModel()->selectedRows()).size()) {
		m_layerTreeView->model()->removeRow(indexes.first().row());
	}
}




//void CUGGIS::on_actionConvexHull_triggered()
//{
//	m_convexHull=new ConvexHull();
//	m_convexHull->show();
//}

void CUGGIS::autoSelectAddedLayer(QList<QgsMapLayer*> layers)
{
	if (!layers.isEmpty()) {
		
		QgsLayerTreeLayer* nodeLayer = QgsProject::instance()->layerTreeRoot()->findLayer(layers[0]->id());
		if (!nodeLayer)
			return;
		auto index = m_layerTreeView->layerTreeModel()->node2index(nodeLayer);
		m_layerTreeView->setCurrentIndex(index);
	}
}

void CUGGIS::initLayerTreeView()
{
	//新建图层树形控件
	m_layerTreeView = new QgsLayerTreeView(this);
	m_layerTreeView->setObjectName(QStringLiteral("theLayerTreeView"));

	//创建QgsLayerTreeModel 实例,并允许用户通过点选复选框手动隐藏/显示图层
	QgsLayerTreeModel* model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
	model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
	model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
	model->setFlag(QgsLayerTreeModel::AllowNodeRename);
	model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
	model->setFlag(QgsLayerTreeModel::UseEmbeddedWidgets);
	model->setFlag(QgsLayerTreeModel::UseTextFormatting);
	model->setAutoCollapseLegendNodes(10);

	//将View视图与Model数据绑定
	m_layerTreeView->setModel(model);

	//将工程实例中的图层根节点与画布绑定，实现与地图画布控件的数据交互
	m_layerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), m_mapCanvas, this);

	// 添加组命令
	QAction* actionAddGroup = new QAction(QStringLiteral("添加组"), this);
	actionAddGroup->setIcon(QIcon(QStringLiteral(":/Resources/mActionAddGroup.png")));
	actionAddGroup->setToolTip(QStringLiteral("添加组"));
	connect(actionAddGroup, &QAction::triggered, m_layerTreeView->defaultActions(), &QgsLayerTreeViewDefaultActions::addGroup);

	// 扩展和收缩图层树
	QAction* actionExpandAll = new QAction(QStringLiteral("展开所有组"), this);
	actionExpandAll->setIcon(QIcon(QStringLiteral(":/Resources/mActionExpandTree.png")));
	actionExpandAll->setToolTip(QStringLiteral("展开所有组"));
	connect(actionExpandAll, &QAction::triggered, m_layerTreeView, &QgsLayerTreeView::expandAllNodes);
	QAction* actionCollapseAll = new QAction(QStringLiteral("折叠所有组"), this);
	actionCollapseAll->setIcon(QIcon(QStringLiteral(":/Resources/mActionCollapseTree.png")));
	actionCollapseAll->setToolTip(QStringLiteral("折叠所有组"));
	connect(actionCollapseAll, &QAction::triggered, m_layerTreeView, &QgsLayerTreeView::collapseAllNodes);

	// 移除图层
	QAction* actionRemoveLayer = new QAction(QStringLiteral("移除图层/组"));
	actionRemoveLayer->setIcon(QIcon(QStringLiteral(":/Resources/mActionRemoveLayer.png")));
	actionRemoveLayer->setToolTip(QStringLiteral("移除图层/组"));
	connect(actionRemoveLayer, &QAction::triggered, this, &CUGGIS::removeLayer);

	QToolBar* toolbar = new QToolBar();
	toolbar->setIconSize(QSize(16, 16));
	toolbar->addAction(actionAddGroup);
	toolbar->addAction(actionExpandAll);
	toolbar->addAction(actionCollapseAll);
	toolbar->addAction(actionRemoveLayer);

	QVBoxLayout* vBoxLayout = new QVBoxLayout();
	vBoxLayout->setMargin(0);
	vBoxLayout->setContentsMargins(0, 0, 0, 0);
	vBoxLayout->setSpacing(0);
	vBoxLayout->addWidget(toolbar);
	vBoxLayout->addWidget(m_layerTreeView);

	QWidget* w = new QWidget;
	w->setLayout(vBoxLayout);
	this->ui.LayerTreeControl->setWidget(w);
	//设置点击右键出现菜单项
	m_layerTreeView->setMenuProvider(new QtGisLayerTreeViewMenuProvider(m_layerTreeView, m_mapCanvas));

	////将View添加到左侧的Dock控件中展示
	//mLayerMapDock->setWidget(w);

	connect(m_layerTreeView, &QgsLayerTreeView::currentLayerChanged, this, &CUGGIS::onCurrentLayerChanged);
	connect(QgsProject::instance()->layerTreeRegistryBridge(), &QgsLayerTreeRegistryBridge::addedLayersToLayerTree, this, &CUGGIS::autoSelectAddedLayer);
}

void CUGGIS::on_actionLayerTreeControl_triggered()
{
	// 打开图层管理器
	if (ui.LayerTreeControl->isVisible())
	{
		ui.LayerTreeControl->setFocus();
	}
	else
	{
		ui.LayerTreeControl->show();
	}
}
