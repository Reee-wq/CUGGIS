
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
#include <QIcon>
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
#include "qstringliteral.h"
#include "QgsLayerTreeView.h"
#include "QgsLayerTreeModel.h"
#include "QgsLayerTreeMapCanvasBridge.h"
#include "QgsLayerTreeRegistryBridge.h"
#include "QgsLayerTree.h"
#include "QgsLayerTreeLayer.h"
#include "QgsLayerTreeViewDefaultActions.h"
#include "QgsLayerTreeUtils.h"
#include "qgsmaptoolzoom.h"

//#include "qgslayerpropertieswidget.h"
#include "qfilesystemmodel.h"






CUGGIS::CUGGIS(QWidget *parent)
    : QMainWindow(parent), treeView(new QTreeView(this)), treeModel(new QStandardItemModel(this))
{
	m_mapCanvas = nullptr;
	// 图层管理器
	m_layerTreeView = nullptr;
	m_layerTreeCanvasBridge = nullptr;
    ui.setupUi(this);
    // 初始化地图画布
	m_mapCanvas = new QgsMapCanvas();
	this->setCentralWidget(m_mapCanvas);
	// 1.2初始化图层管理器,new了两次导致白框bug
	//m_layerTreeView = new QgsLayerTreeView(this);
	initLayerTreeView();
	m_curMapLayer = nullptr;
	//1.2从文件树目录拖拽文件并打开
	formatList << "tif" << "tiff" << "hdf" << "shp" << "qgs" << "qgz" << "jpg" << "png" << "bmp";
	//1.2工具箱树目录
	  // 设置树视图和模型
	treeView->setModel(treeModel);
	treeView->setRootIsDecorated(true);
	// 禁用编辑模式
	treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	treeView->setSelectionMode(QAbstractItemView::SingleSelection); // 允许单选
	treeView->setFocusPolicy(Qt::StrongFocus); // 确保获得焦点

	// 1.2工具箱创建侧边栏
	QDockWidget* dockWidget = new QDockWidget("Tool Box", this);
	dockWidget->setWidget(treeView);
	addDockWidget(Qt::RightDockWidgetArea, dockWidget);

	// 1.2工具箱创建QAction
	QAction* action1 = new QAction( "GeoTransformTool", this);
	connect(action1, &QAction::triggered, this, &CUGGIS::onAction1Triggered);
	QAction* action2 = new QAction("GeoProcessTool", this);
	connect(action2, &QAction::triggered, this, &CUGGIS::onAction2Triggered);
	QAction* action3 = new QAction("MapEdit", this);
	connect(action3, &QAction::triggered, this, &CUGGIS::onAction2Triggered);
	QAction* action4 = new QAction("LayerTool", this);
	connect(action4, &QAction::triggered, this, &CUGGIS::onAction2Triggered);
	QAction* action5 = new QAction("RasterAnalyze", this);
	connect(action5, &QAction::triggered, this, &CUGGIS::onAction2Triggered);


	// 1.2工具箱向模型中添加父图层并添加子图层
	addLayer("GeoTransformTool", action1, { ui.actionExcelShp,ui.actionSHPtoGRID });
	addLayer("GeoProcessTool", action2, {ui.actionBuffer,ui.actionConvexHull,ui.actionRectangle,ui.actionCircle,ui.actionRandom });
	addLayer("MapEdit", action3, { ui.actionAddPoint ,ui.actionDelete, ui.actionMove,ui.actionCopy,ui.actionBuffer_2,ui.actionParallel });
	addLayer("LayerTool", action4, { ui.actionAddVectorLayer,ui.actionAddRasterLayer,ui.actionLayerProperties,ui.actionQgsStylelibMng,ui.actionSelfStylelibMng });
	addLayer("RasterAnalyze", action5, { ui.actionRasterCalculator });
	// 在构造函数中连接 doubleClicked 信号
	connect(treeView, &QTreeView::doubleClicked, this, &CUGGIS::onTreeViewDoubleClick);
	connect(ui.actionFileBrowser, &QAction::triggered, ui.fileTree, &QDockWidget::show);
	connect(ui.actionLayer, &QAction::triggered, ui.LayerTreeControl, &QDockWidget::show);
	connect(ui.actionToolBoxs, &QAction::triggered, dockWidget, &QDockWidget::show);

	//1.6放大缩小工具
	m_zoomInTool = new QgsMapToolZoom(m_mapCanvas, false);
	m_zoomOutTool = new QgsMapToolZoom(m_mapCanvas, true);
	//矢量，地理处理
	//凸包
	m_convexHull = nullptr;
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
	m_treeView->setRootIndex(m_model->index("D:/"));
	// 1.2设置 QTreeView 接受拖拽操作,支持从文件树目录拖入文件
	m_treeView->setDragEnabled(true);
	m_treeView->setAcceptDrops(true);
	ui.fileTree->setWidget(m_treeView);
	//1.2允许窗口可以拖入文件（放入主窗口构造函数中）
	this->setAcceptDrops(true);
	
	// 假设您已经在您的主窗口类中设置了 ui->statusBar()
	QLabel* coordinateLabel = new QLabel(this);
	ui.statusBar->addWidget(coordinateLabel);

	// 连接 QgsMapCanvas 的 xyCoordinates 信号到您的槽函数
	connect(m_mapCanvas, &QgsMapCanvas::xyCoordinates, this, [this, coordinateLabel](const QgsPointXY& coordinate) {
		QString crsAuthId = m_mapCanvas->mapSettings().destinationCrs().authid();
		QString displayText = coordinate.toString().replace(" ", ", ").prepend("CRS: " + crsAuthId + " - ");
		coordinateLabel->setText(displayText);
		});
	//数据处理，C4.5
	jctreeView=ui.jctreeView;
}

CUGGIS::~CUGGIS()
{
	//delete m_ID3andC4->pTree;
}

void CUGGIS::onCurrentLayerChanged(QgsMapLayer* layer)//检查给定的图层是否为特定类型的矢量图层（例如Shapefile），并将其路径存储在成员变量m_curMapLayer中。
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
	qDebug() << "current layer:" << m_curMapLayer->name();
	QgsProject::instance()->addMapLayers(layerList);
	// 添加图层到图层树
	/*addLayersBelowLast(layerList);*/
	// 执行动态投影
	QgsCoordinateReferenceSystem firstCrs = m_curMapLayer->crs();
	qDebug() << firstCrs.description();
	//setLayersCrsToLastLayerCrs();
	m_mapCanvas->refresh();
	qDebug() << "current layer:" << m_curMapLayer->crs();
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





void CUGGIS::autoSelectAddedLayer(QList<QgsMapLayer*> layers)//找到列表中的第一个图层，并将其设置为当前活动的图层
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
	actionAddGroup->setIcon(QIcon(QStringLiteral("./Resources/mActionAddGroup.png")));
	actionAddGroup->setToolTip(QStringLiteral("Add Group"));
	connect(actionAddGroup, &QAction::triggered, m_layerTreeView->defaultActions(), &QgsLayerTreeViewDefaultActions::addGroup);

	// 扩展和收缩图层树
	QAction* actionExpandAll = new QAction(QStringLiteral("展开所有组"), this);
	actionExpandAll->setIcon(QIcon(QStringLiteral("./Resources/mActionExpandTree.png")));
	actionExpandAll->setToolTip(QStringLiteral("Unfold All Groups"));
	connect(actionExpandAll, &QAction::triggered, m_layerTreeView, &QgsLayerTreeView::expandAllNodes);
	QAction* actionCollapseAll = new QAction(QStringLiteral("折叠所有组"), this);
	actionCollapseAll->setIcon(QIcon(QStringLiteral("./Resources/mActionCollapseTree.png")));
	actionCollapseAll->setToolTip(QStringLiteral("Fold All Groups"));
	connect(actionCollapseAll, &QAction::triggered, m_layerTreeView, &QgsLayerTreeView::collapseAllNodes);

	// 移除图层
	QAction* actionRemoveLayer = new QAction(QStringLiteral("移除图层/组"));
	actionRemoveLayer->setIcon(QIcon(QStringLiteral("./Resources/mActionRemoveLayer.png")));
	actionRemoveLayer->setToolTip(QStringLiteral("Remove Layer/Group"));
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
