#include "QtGisLayerTreeViewMenuProvider.h"
#include "CUGGIS.h"

#include "qmenu.h"
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QTableWidget>
#include "QgsLayerTreeView.h"
#include "QgsMapCanvas.h"
#include "QgsMapLayer.h"
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
		QAction* removeGroupAction = mView->defaultActions()->actionRemoveGroupOrLayer(menu);
		removeGroupAction->setIcon(QIcon(QStringLiteral("./Resources/mActionRemoveLayer.png"))); // ����ͼ��
		menu->addAction(removeGroupAction);
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
		// ��ӵ�����Ϣ
		qDebug() << "vLayer is valid:" << vLayer;
		
		if (vLayer) {
			menu->addAction(mView->defaultActions()->actionShowFeatureCount(menu));
			// ���� "&Label" ���������ӵ�˽�вۺ���
			QAction* labelAction = new QAction("&Label", menu);
            			labelAction->setIcon(QIcon(QStringLiteral("./Resources/Label.png"))); // ����ͼ��
			menu->addAction(labelAction);
			QObject::connect(labelAction, &QAction::triggered, this, &QtGisLayerTreeViewMenuProvider::slot_labelShowAction);

			// ���� "&Attribute Table" ���������ӵ�˽�вۺ���
			
				QAction* attrTableAction = new QAction("&Show Attribute Table", menu);
				attrTableAction->setIcon(QIcon(QStringLiteral("./Resources/OpenAttrTable.png"))); // ����ͼ��
				menu->addAction(attrTableAction);
				QObject::connect(attrTableAction, &QAction::triggered, this, &QtGisLayerTreeViewMenuProvider::slot_OpenAttributeAction);
			
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

		//���ұ�ǩ��ʾ��Ϣ
		QMap<QgsVectorLayer*, StLabelShowInfo>::iterator it = m_mapLabelshowInfo.find(veclayer);  //��ǩ��ʾ��Ϣ
		StLabelShowInfo st;
		if (it != m_mapLabelshowInfo.end())
			st = it.value();
		else
		{
			st.bshow = false;
			st.name = "";
		}

		QStringList items = veclayer->fields().names();//��ȡʸ��ͼ����ֶ����ƣ�
		if (m_dlgLabelctrl == nullptr)//������ǩ���ƶԻ���
		{
			//�����ǩ���ƶԻ���m_dlgLabelctrl����δ�������򴴽�һ���µĶԻ���
			//�������źźͲۣ��Ա��ڶԻ����еİ�ť���ʱ������Ӧ�Ĳۺ���
			m_dlgLabelctrl = new LabelControl();
			connect(m_dlgLabelctrl, SIGNAL(sig_labelctrlBtnClicked(int, bool, QString)), this, SLOT(slot_labelctrlChange(int, bool, QString)));
		}
		//���ñ�ǩ���ƶԻ���ĳ�ʼ��Ϣ
		m_dlgLabelctrl->SetItems(items);
		m_dlgLabelctrl->SetInitInfo(st.bshow, st.name);
		//��ʾ�������ǩ���ƶԻ���
		m_dlgLabelctrl->show();
		m_dlgLabelctrl->raise();
		m_dlgLabelctrl->activateWindow();
	}

}
void QtGisLayerTreeViewMenuProvider::slot_OpenAttributeAction()
{
	QgsMapLayer* layer = mView->currentLayer();
	if (!layer && layer->type() != QgsMapLayerType::VectorLayer)
	{

		QMessageBox::warning(nullptr, "����", "��ѡ����Ч��ʸ��ͼ��");
		return;
	}

	QgsVectorLayer* vectorLayer = static_cast<QgsVectorLayer*>(layer);

	QDialog dialog(nullptr);
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
		QMessageBox::information(nullptr, "�ɹ�", "���Ա��ѱ���");
		});

	dialog.exec();
}
void QtGisLayerTreeViewMenuProvider::slot_labelctrlChange(int id, bool bchange, QString name) {
	if (m_dlgLabelctrl)
	{
		//�����Ϊ�գ������ظÿؼ���ɾ������Ȼ��m_dlgLabelctrl����Ϊ0������ָ�룩��
		m_dlgLabelctrl->hide();
		delete m_dlgLabelctrl;
		m_dlgLabelctrl = 0;
	}

	if (id == 1)
	{
		//��鴫���id�Ƿ�Ϊ1������ǣ����ȡ��ǰ��ͼ�еĵ�ǰͼ��
		QgsMapLayer* ll = mView->currentLayer();
		if (ll && ll->type() == QgsMapLayerType::VectorLayer)
		{
			QgsVectorLayer* vecLayer = qobject_cast<QgsVectorLayer*>(ll);

			//��ǩ��ʾ��Ϣ��ѯ
			QMap<QgsVectorLayer*, StLabelShowInfo>::iterator it = m_mapLabelshowInfo.find(vecLayer);  //��ǩ��ʾ��Ϣ
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
				//��ʾ��ǩ
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

