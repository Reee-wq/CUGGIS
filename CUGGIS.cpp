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
	// ͼ�������
	m_layerTreeView = nullptr;
	m_layerTreeCanvasBridge = nullptr;
    ui.setupUi(this);
    	// ��ʼ����ͼ����
	m_mapCanvas = new QgsMapCanvas();
	this->setCentralWidget(m_mapCanvas);
	// ��ʼ��ͼ�������
	//m_layerTreeView = new QgsLayerTreeView(this);
	initLayerTreeView();
	m_curMapLayer = nullptr;
	//ʸ����������
	//͹��
	//m_convexHull = nullptr;
	//���ݴ���ID3andC4
	m_ID3andC4 = nullptr;
	connect(ui.actionQgsStylelibMng, &QAction::triggered, this, &CUGGIS::on_actionQgsStylelibMng_triggered);
	connect(ui.actionSelfStylelibMng, &QAction::triggered, this, &CUGGIS::on_actionSelfStylelibMng_triggered);
	connect(m_layerTreeView, &QgsLayerTreeView::doubleClicked, this, &CUGGIS::onLayerTreeItemDoubleClicked);
	// �����ļ�ϵͳģ��
	QFileSystemModel* m_model = new QFileSystemModel(this);

	// ���� QTreeView
	QTreeView* m_treeView = new QTreeView(this);
	m_model->setRootPath("D:/");

	// �����ļ�ϵͳģ��
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
		//�ж��Ƿ�Ϊ����Ҫ�����ͼ�����ͣ�����shapefile
		if (vectorLayer->dataProvider()->name() == "ogr" && vectorLayer->source().endsWith(".shp"))
		{
			//��ȡʸ��ͼ���·��
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
		QString layerBaseName = fi.baseName(); // ͼ������

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
	
	//ѡ��ͼ��
	connect(m_layerTreeView, &QgsLayerTreeView::currentLayerChanged, this, &CUGGIS::onCurrentLayerChanged);

	m_curMapLayer = layerList[0];
	QgsProject::instance()->addMapLayers(layerList);
	// ���ͼ�㵽ͼ����
	//addLayersBelowLast(layerList);
	// ִ�ж�̬ͶӰ
	//setLayersCrsToLastLayerCrs();
	m_mapCanvas->refresh();
}

void CUGGIS::on_actionAddRasterLayer_triggered()
{
	QStringList layerPathList = QFileDialog::getOpenFileNames(this, QStringLiteral("ѡ��դ������"), "", "Image (*.img *.tif *.tiff)");
	QList<QgsMapLayer*> layerList;
	for (const QString& layerPath : layerPathList)
	{
		QFileInfo fi(layerPath);
		if (!fi.exists()) { return; }
		QString layerBaseName = fi.baseName(); // ͼ������

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
		// ���ͼ������
		if (m_curMapLayer->type() == QgsMapLayerType::VectorLayer)
		{
			// ��ʸ��ͼ�����Թ�����
			VectorLayerPropertiesWidget* widget = new VectorLayerPropertiesWidget(m_curMapLayer, this);
			widget->exec();
		}
		else if (m_curMapLayer->type() == QgsMapLayerType::RasterLayer)
		{
			// ��դ��ͼ�����Թ�����
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
	//�½�ͼ�����οؼ�
	m_layerTreeView = new QgsLayerTreeView(this);
	m_layerTreeView->setObjectName(QStringLiteral("theLayerTreeView"));

	//����QgsLayerTreeModel ʵ��,�������û�ͨ����ѡ��ѡ���ֶ�����/��ʾͼ��
	QgsLayerTreeModel* model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
	model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
	model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
	model->setFlag(QgsLayerTreeModel::AllowNodeRename);
	model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
	model->setFlag(QgsLayerTreeModel::UseEmbeddedWidgets);
	model->setFlag(QgsLayerTreeModel::UseTextFormatting);
	model->setAutoCollapseLegendNodes(10);

	//��View��ͼ��Model���ݰ�
	m_layerTreeView->setModel(model);

	//������ʵ���е�ͼ����ڵ��뻭���󶨣�ʵ�����ͼ�����ؼ������ݽ���
	m_layerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), m_mapCanvas, this);

	// ���������
	QAction* actionAddGroup = new QAction(QStringLiteral("�����"), this);
	actionAddGroup->setIcon(QIcon(QStringLiteral(":/Resources/mActionAddGroup.png")));
	actionAddGroup->setToolTip(QStringLiteral("�����"));
	connect(actionAddGroup, &QAction::triggered, m_layerTreeView->defaultActions(), &QgsLayerTreeViewDefaultActions::addGroup);

	// ��չ������ͼ����
	QAction* actionExpandAll = new QAction(QStringLiteral("չ��������"), this);
	actionExpandAll->setIcon(QIcon(QStringLiteral(":/Resources/mActionExpandTree.png")));
	actionExpandAll->setToolTip(QStringLiteral("չ��������"));
	connect(actionExpandAll, &QAction::triggered, m_layerTreeView, &QgsLayerTreeView::expandAllNodes);
	QAction* actionCollapseAll = new QAction(QStringLiteral("�۵�������"), this);
	actionCollapseAll->setIcon(QIcon(QStringLiteral(":/Resources/mActionCollapseTree.png")));
	actionCollapseAll->setToolTip(QStringLiteral("�۵�������"));
	connect(actionCollapseAll, &QAction::triggered, m_layerTreeView, &QgsLayerTreeView::collapseAllNodes);

	// �Ƴ�ͼ��
	QAction* actionRemoveLayer = new QAction(QStringLiteral("�Ƴ�ͼ��/��"));
	actionRemoveLayer->setIcon(QIcon(QStringLiteral(":/Resources/mActionRemoveLayer.png")));
	actionRemoveLayer->setToolTip(QStringLiteral("�Ƴ�ͼ��/��"));
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
	//���õ���Ҽ����ֲ˵���
	m_layerTreeView->setMenuProvider(new QtGisLayerTreeViewMenuProvider(m_layerTreeView, m_mapCanvas));

	////��View��ӵ�����Dock�ؼ���չʾ
	//mLayerMapDock->setWidget(w);

	connect(m_layerTreeView, &QgsLayerTreeView::currentLayerChanged, this, &CUGGIS::onCurrentLayerChanged);
	connect(QgsProject::instance()->layerTreeRegistryBridge(), &QgsLayerTreeRegistryBridge::addedLayersToLayerTree, this, &CUGGIS::autoSelectAddedLayer);
}

void CUGGIS::on_actionLayerTreeControl_triggered()
{
	// ��ͼ�������
	if (ui.LayerTreeControl->isVisible())
	{
		ui.LayerTreeControl->setFocus();
	}
	else
	{
		ui.LayerTreeControl->show();
	}
}
