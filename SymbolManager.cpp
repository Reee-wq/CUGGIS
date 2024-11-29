#include "SymbolManager.h"


SymbolManager::SymbolManager(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QgsGui::instance()->enableAutoGeometryRestore(this); //��ס���ڴ�С
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	mStyle = StyleManager::getStyle(); //��ȡ��ʽ

	//������״��ͼ
	ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers); //��ֹ�༭
	ui.treeView->setHeaderHidden(true); //���ر�ͷ
	double treeIconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().horizontalAdvance('X') * 10;
	ui.treeView->setIconSize(QSize(static_cast<int>(treeIconSize), static_cast<int>(treeIconSize)));//����ͼ���С
	QStandardItemModel* groupModel = new QStandardItemModel(ui.treeView); //������״ģ��
	ui.treeView->setModel(groupModel); //����ģ��
	ui.treeView->setHeaderHidden(true); //���ر�ͷ
	setTreeModel();


	connect(ui.tabWidget, &QTabWidget::currentChanged, this, &SymbolManager::tabItemTypeChanged);

	ui.treeView->setCurrentIndex(ui.treeView->model()->index(0, 0)); //ѡ�е�һ���ڵ�
	connect(ui.treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &SymbolManager::groupChanged);
	mModel = new QgsStyleProxyModel(mStyle);
	//����ͼ���С
	double iconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().maxWidth() * 10;
	//�����б���ͼ
	ui.listView->setIconSize(QSize(static_cast<int>(iconSize * 0.5), static_cast<int>(iconSize * 0.5)));
	//�����б���ͼ�������С
	ui.listView->setGridSize(QSize(static_cast<int>(iconSize * 0.55), static_cast<int>(iconSize * 0.85)));

	mModel->addDesiredIconSize(ui.listView->iconSize());
	mModel->addDesiredIconSize(ui.treeView->iconSize());

	ui.listView->setModel(mModel);
	ui.listView->setSelectionBehavior(QAbstractItemView::SelectItems);
	ui.listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui.treeView->setSelectionModel(ui.listView->selectionModel());
	ui.treeView->setSelectionMode(ui.listView->selectionMode());

	ui.tabWidget->setCurrentIndex(1);
	tabItemTypeChanged();
	for (int i = 0; i < ui.listView->model()->rowCount(); i++)
	{
		mModel->data(ui.listView->model()->index(i, 0), 1);  //Ԥ����ͼ��
	}
	ui.tabWidget->setCurrentIndex(2);
	tabItemTypeChanged();
	for (int i = 0; i < ui.listView->model()->rowCount(); i++)
	{
		mModel->data(ui.listView->model()->index(i, 0), 1);  //Ԥ����ͼ��
	}
	ui.tabWidget->setCurrentIndex(3);
	tabItemTypeChanged();
	for (int i = 0; i < ui.listView->model()->rowCount(); i++)
	{
		mModel->data(ui.listView->model()->index(i, 0), 1);  //Ԥ����ͼ��
	}

	ui.tabWidget->setCurrentIndex(0);
	tabItemTypeChanged();
	connect(ui.buttonClose, &QPushButton::clicked, this, &SymbolManager::hide);
	ui.treeView->resizeColumnToContents(0); //�����п�
	QApplication::restoreOverrideCursor(); //�ָ������ʽ

	connect(ui.buttonImport, &QPushButton::clicked, this, &SymbolManager::buttonImport);

	setTreeModel(); //������״ģ��

	connect(ui.buttonExport, &QPushButton::clicked, this, &SymbolManager::buttonExport);
}

SymbolManager::~SymbolManager()
{}

//���������״��ͼ
void SymbolManager::setTreeModel() {
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui.treeView->model());
	model->clear();

	QStandardItem* allSymbols = new QStandardItem(QStringLiteral("ȫ��"));
	allSymbols->setData("all");
	allSymbols->setEditable(false);
	QFont font = allSymbols->font();
	font.setBold(true);
	allSymbols->setFont(font);
	model->appendRow(allSymbols);

	QStandardItem* favoriteSymbols = new QStandardItem(QStringLiteral("�ղ�"));
	favoriteSymbols->setData("favorite");
	favoriteSymbols->setEditable(false);
	font = favoriteSymbols->font();
	font.setBold(true);
	favoriteSymbols->setFont(font);
	model->appendRow(favoriteSymbols);

	QStandardItem* taggroup = new QStandardItem(QString()); //���ÿ�
	taggroup->setData("tags");
	taggroup->setEditable(false);
	QStringList tags = mStyle->tags();
	tags.sort();
	for (const QString& tag : qgis::as_const(tags))
	{
		QStandardItem* item = new QStandardItem(tag);
		item->setData(mStyle->tagId(tag));
		item->setEditable(true);
		taggroup->appendRow(item);
	}
	taggroup->setText(QStringLiteral("��ǩ"));
	font = taggroup->font();
	font.setBold(true);
	taggroup->setFont(font);
	model->appendRow(taggroup);

	//��չ��С
	int rows = model->rowCount(model->indexFromItem(model->invisibleRootItem()));
	for (int i = 0; i < rows; i++)
	{
		ui.treeView->setExpanded(model->indexFromItem(model->item(i)), true);
	}
}

//�����Ҳ��б���ͼ
void SymbolManager::tabItemTypeChanged()
{
	int type = 0;
	if (ui.tabWidget->currentIndex() == 0)
	{
		type = QgsSymbol::Marker;
	}
	else if (ui.tabWidget->currentIndex() == 1)
	{
		type = QgsSymbol::Line;
	}
	else if (ui.tabWidget->currentIndex() == 2)
	{
		type = QgsSymbol::Fill;
	}
	else if (ui.tabWidget->currentIndex() == 3)
	{
		type = 3;
	}
	const bool isSymbol = type != 3;
	mModel->setEntityFilter(isSymbol ? QgsStyle::SymbolEntity : QgsStyle::ColorrampEntity);  // ����ʵ�������
	if (type != 3)
	{
		mModel->setEntityFilterEnabled(true); // �������͹�����
		mModel->setSymbolTypeFilterEnabled(true); // �������͹�����
		mModel->setSymbolType(static_cast<QgsSymbol::SymbolType>(type)); // ���÷�������

	}
	else
	{
		mModel->setEntityFilterEnabled(true);
		mModel->setSymbolTypeFilterEnabled(false);
	}

	QModelIndex index = ui.treeView->selectionModel()->currentIndex();
	groupChanged(index);
}

//�����Ҳ��б���ͼ
void SymbolManager::groupChanged(const QModelIndex& index)
{
	QStringList groupSymbols;

	const QString category = index.data(Qt::UserRole + 1).toString();
	if (category == QLatin1String("all") || category == QLatin1String("tags"))
	{
		mModel->setTagId(-1); // ȫ��
		mModel->setSmartGroupId(-1);
		mModel->setFavoritesOnly(false);
	}
	else if (category == QLatin1String("favorite"))
	{
		mModel->setTagId(-1);
		mModel->setSmartGroupId(-1);
		mModel->setFavoritesOnly(true); // �ղ�
	}
	else // ����
	{
		int tagId = index.data(Qt::UserRole + 1).toInt();
		mModel->setTagId(tagId);
		mModel->setSmartGroupId(-1);
		mModel->setFavoritesOnly(false);
	}
}

//������ſ�
void SymbolManager::buttonImport() {
	bool isDelete = false;  //�Ƿ�ɾ����ǰ���ſ�
	if (mStyle->symbolCount() > 0) {
		int res = QMessageBox::warning(this, QStringLiteral("������ſ�"), QStringLiteral("�Ƿ���Ҫ�����ǰ���ſ��ٵ��룿"), QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
		if (res == QMessageBox::StandardButton::Yes) { //������ſ�
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			isDelete = true;
			deleteSymbols(QgsStyle::StyleEntity::SymbolEntity, mStyle);
			deleteSymbols(QgsStyle::StyleEntity::TagEntity, mStyle);
			mStyle->clear();
			setTreeModel(); //������״ģ��
			tabItemTypeChanged();
			QApplication::restoreOverrideCursor(); //�ָ������ʽ
		}
	}

	QStringList fileNames = QFileDialog::getOpenFileNames(this, QStringLiteral("������ſ�"), "./res", QStringLiteral("XML �ļ� (*.xml)"));
	if (fileNames.size() < 1)
	{
		return;
	}
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	for (int i = 0; i < fileNames.size(); i++)
	{
		bool status = mStyle->importXml(fileNames[i]);
		if (!status)
		{
			QApplication::restoreOverrideCursor();
			QFileInfo file(fileNames[i]);
			QMessageBox::warning(this, QStringLiteral("������ſ�"), QStringLiteral("���롰%1�����������Ի��߼���ļ���").arg(file.baseName()));
		}
		if (isDelete)
		{
			if (mModel != nullptr)
			{
				delete mModel;
				mModel = nullptr;
			}
			mModel = new QgsStyleProxyModel(mStyle);
			mModel->addDesiredIconSize(ui.listView->iconSize());
			mModel->addDesiredIconSize(ui.treeView->iconSize());
			ui.listView->setModel(mModel);
		}

	}
	setTreeModel(); //������״ģ��
	//���ؼ���ʱ��
	ui.tabWidget->setCurrentIndex(2);
	ui.treeView->setCurrentIndex(ui.treeView->model()->index(0, 0));
	int count = ((QStandardItemModel*)ui.listView->model())->rowCount();
	for (int i = 0; i < count; i++)
	{
		mModel->data(((QStandardItemModel*)ui.listView->model())->index(i, 0), 1);
	}
	ui.tabWidget->setCurrentIndex(0);
	tabItemTypeChanged();

	ui.listView->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	QApplication::restoreOverrideCursor();
	ui.treeView->resizeColumnToContents(0);
	QMessageBox::warning(this, QStringLiteral("������ſ�"), QStringLiteral("������ɣ�"));
}


//ɾ������
bool SymbolManager::deleteSymbols(QgsStyle::StyleEntity type, QgsStyle* style) {
	QStringList names = style->allNames(type); //��ȡ���з�������
	bool groupRemoved = false; //�Ƿ�ɾ������
	QString query; //ɾ�����
	switch (type) //������������ɾ�����
	{
	case QgsStyle::StyleEntity::SymbolEntity:
		query = QgsSqlite3Mprintf("DELETE FROM symbol; DELETE FROM tagmap");
		break;
	case QgsStyle::StyleEntity::ColorrampEntity:
		query = QgsSqlite3Mprintf("DELETE FROM colorramp");
		break;
	case QgsStyle::StyleEntity::TextFormatEntity:
		query = QgsSqlite3Mprintf("DELETE FROM textformat");
		break;
	case QgsStyle::StyleEntity::LabelSettingsEntity:
		query = QgsSqlite3Mprintf("DELETE FROM labelsettings");
		break;
	case QgsStyle::StyleEntity::TagEntity:
		query = QgsSqlite3Mprintf("DELETE FROM tag");
		groupRemoved = true;
		break;
	case QgsStyle::StyleEntity::SmartgroupEntity:
		query = QgsSqlite3Mprintf("DELETE FROM smartgroup");
		groupRemoved = true;
		break;
	}

	bool result = false;

	bool noError = true;
	sqlite3_database_unique_ptr mCurrentDB;
	mCurrentDB.open(QgsApplication::userStylePath());
	if (!mCurrentDB)
		noError = false;

	char* zErr = nullptr;
	int nErr = sqlite3_exec(mCurrentDB.get(), query.toUtf8().constData(), nullptr, nullptr, &zErr);//ִ��ɾ�����

	if (nErr != SQLITE_OK)
	{
		sqlite3_free(zErr);
		noError = false;
	}

	if (!noError)
	{
		return false;
	}
	else
	{

		if (groupRemoved)
		{
			QgsSettings settings;
			settings.setValue(QStringLiteral("qgis/symbolsListGroupsIndex"), 0);
			if (type == QgsStyle::StyleEntity::SymbolEntity)
			{
				for (int i = 0; i < names.size(); i++)
				{

					emit style->symbolRemoved(names[i]);
				}
			}
			emit style->groupsModified();
		}
		result = true;
	}
	return result;
}

//�������ſ�
void SymbolManager::buttonExport() {
	if (!mSymbolExport) {
		mSymbolExport = new SymbolExport(this);
	}
	mSymbolExport->show();
}





