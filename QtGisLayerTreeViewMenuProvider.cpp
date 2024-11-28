#include "QtGisLayerTreeViewMenuProvider.h"
#include "CUGGIS.h"

#include "qmenu.h"

#include "QgsLayerTreeView.h"
#include "QgsMapCanvas.h"
#include "QgsLayerTreeNode.h"
#include "QgsLayerTree.h"
#include "QgsVectorLayer.h"
#include "QgsLayerTreeViewDefaultActions.h"
#include "qgsvectorlayerproperties.h"
#include"labelcontrol.h"
#include "qgspallabeling.h"
#include "qgsvectorlayerlabeling.h"


QtGisLayerTreeViewMenuProvider::QtGisLayerTreeViewMenuProvider(QgsLayerTreeView* layerTreeView, QgsMapCanvas* mapCanvas)
	:QgsLayerTreeViewMenuProvider()
	, mMapCanvas(mapCanvas)
	, mView(layerTreeView)
{
	
}

QtGisLayerTreeViewMenuProvider::~QtGisLayerTreeViewMenuProvider()
{
}

QMenu* QtGisLayerTreeViewMenuProvider::createContextMenu()
{
	QMenu* menu = new QMenu;
	QModelIndex index = mView->currentIndex();
	if (!index.isValid()) return nullptr;
	QgsLayerTreeNode* node = mView->layerTreeModel()->index2node(index);
	if (QgsLayerTree::isGroup(node)) {
		menu->addAction(mView->defaultActions()->actionRemoveGroupOrLayer(menu));
		menu->addAction(mView->defaultActions()->actionRenameGroupOrLayer(menu));
	}
	else if(QgsLayerTree::isLayer(node)){
		QgsMapLayer* layer = QgsLayerTree::toLayer(node)->layer();
		if (layer) {
			menu->addAction(mView->defaultActions()->actionZoomToLayer(mMapCanvas,menu));
			menu->addAction(mView->defaultActions()->actionRemoveGroupOrLayer(menu));
			menu->addAction(mView->defaultActions()->actionRenameGroupOrLayer(menu));
		}
		QgsVectorLayer* vLayer = qobject_cast<QgsVectorLayer*>(layer);
		// 添加调试信息
		qDebug() << "vLayer is valid:" << vLayer;
		
		if (vLayer) {
			menu->addAction(mView->defaultActions()->actionShowFeatureCount(menu));
			// 创建 "&Label" 动作并连接到私有槽函数
			QAction* labelAction = new QAction("&Label", menu);
			menu->addAction(labelAction);
			QObject::connect(labelAction, &QAction::triggered, this, &QtGisLayerTreeViewMenuProvider::slot_labelShowAction);
		}
	}

	return menu;
}

void QtGisLayerTreeViewMenuProvider::slot_labelShowAction()
{

	QgsMapLayer* layer = mView->currentLayer();
	if (layer && layer->type() == QgsMapLayerType::VectorLayer)
	{
		QgsVectorLayer* veclayer = qobject_cast<QgsVectorLayer*>(layer);

		//查找标签显示信息
		QMap<QgsVectorLayer*, StLabelShowInfo>::iterator it = m_mapLabelshowInfo.find(veclayer);  //标签显示信息
		StLabelShowInfo st;
		if (it != m_mapLabelshowInfo.end())
			st = it.value();
		else
		{
			st.bshow = false;
			st.name = "";
		}

		QStringList items = veclayer->fields().names();
		if (m_dlgLabelctrl == nullptr)
		{
			m_dlgLabelctrl = new LabelControl();
			connect(m_dlgLabelctrl, SIGNAL(sig_labelctrlBtnClicked(int, bool, QString)), this, SLOT(slot_labelctrlChange(int, bool, QString)));
		}
		m_dlgLabelctrl->SetItems(items);
		m_dlgLabelctrl->SetInitInfo(st.bshow, st.name);
		m_dlgLabelctrl->show();
		m_dlgLabelctrl->raise();
		m_dlgLabelctrl->activateWindow();
	}

}
void QtGisLayerTreeViewMenuProvider::slot_labelctrlChange(int id, bool bchange, QString name) {
	if (m_dlgLabelctrl)
	{
		m_dlgLabelctrl->hide();
		delete m_dlgLabelctrl;
		m_dlgLabelctrl = 0;
	}

	if (id == 1)
	{
		QgsMapLayer* ll = mView->currentLayer();
		if (ll && ll->type() == QgsMapLayerType::VectorLayer)
		{
			QgsVectorLayer* vecLayer = qobject_cast<QgsVectorLayer*>(ll);

			//标签显示信息查询
			QMap<QgsVectorLayer*, StLabelShowInfo>::iterator it = m_mapLabelshowInfo.find(vecLayer);  //标签显示信息
			StLabelShowInfo st;
			if (it != m_mapLabelshowInfo.end())
			{
				st = it.value();
				m_mapLabelshowInfo.erase(it);
			}
			st.bshow = bchange;
			st.name = name;
			m_mapLabelshowInfo.insert(vecLayer, st);


			if (bchange)
			{
				//显示标签
				QgsPalLayerSettings layerSettings;
				layerSettings.drawLabels = true;
				layerSettings.fieldName = name;
				layerSettings.isExpression = false;
				layerSettings.placement = QgsPalLayerSettings::OverPoint;
				layerSettings.yOffset = 2.50;

				QgsTextBufferSettings buffersettings;
				buffersettings.setEnabled(false);
				buffersettings.setSize(1);
				buffersettings.setColor(QColor());

				QgsTextFormat format;
				QFont font("SimSun", 5, 5, false);
				font.setUnderline(false);
				format.setFont(font);
				format.setBuffer(buffersettings);
				layerSettings.setFormat(format);

				QgsVectorLayerSimpleLabeling* labeling = new QgsVectorLayerSimpleLabeling(layerSettings);
				vecLayer->setLabeling(labeling);
				vecLayer->setLabelsEnabled(true);
			}
			else
			{
				vecLayer->setLabelsEnabled(false);
			}
			mMapCanvas->refresh();
		} //vecterlayer

	}// change
	
}

