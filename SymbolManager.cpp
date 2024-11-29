#include "SymbolManager.h"


SymbolManager::SymbolManager(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QgsGui::instance()->enableAutoGeometryRestore(this); //记住窗口大小
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	mStyle = StyleManager::getStyle(); //获取样式

	//设置树状视图
	ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
	ui.treeView->setHeaderHidden(true); //隐藏表头
	double treeIconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().horizontalAdvance('X') * 10;
	ui.treeView->setIconSize(QSize(static_cast<int>(treeIconSize), static_cast<int>(treeIconSize)));//设置图标大小
	QStandardItemModel* groupModel = new QStandardItemModel(ui.treeView); //创建树状模型
	ui.treeView->setModel(groupModel); //设置模型
	ui.treeView->setHeaderHidden(true); //隐藏表头
	setTreeModel();


	connect(ui.tabWidget, &QTabWidget::currentChanged, this, &SymbolManager::tabItemTypeChanged);

	ui.treeView->setCurrentIndex(ui.treeView->model()->index(0, 0)); //选中第一个节点
	connect(ui.treeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &SymbolManager::groupChanged);
	mModel = new QgsStyleProxyModel(mStyle);
	//设置图标大小
	double iconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().maxWidth() * 10;
	//设置列表视图
	ui.listView->setIconSize(QSize(static_cast<int>(iconSize * 0.5), static_cast<int>(iconSize * 0.5)));
	//设置列表视图的网格大小
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
		mModel->data(ui.listView->model()->index(i, 0), 1);  //预加载图标
	}
	ui.tabWidget->setCurrentIndex(2);
	tabItemTypeChanged();
	for (int i = 0; i < ui.listView->model()->rowCount(); i++)
	{
		mModel->data(ui.listView->model()->index(i, 0), 1);  //预加载图标
	}
	ui.tabWidget->setCurrentIndex(3);
	tabItemTypeChanged();
	for (int i = 0; i < ui.listView->model()->rowCount(); i++)
	{
		mModel->data(ui.listView->model()->index(i, 0), 1);  //预加载图标
	}

	ui.tabWidget->setCurrentIndex(0);
	tabItemTypeChanged();
	connect(ui.buttonClose, &QPushButton::clicked, this, &SymbolManager::hide);
	ui.treeView->resizeColumnToContents(0); //调整列宽
	QApplication::restoreOverrideCursor(); //恢复鼠标样式

	connect(ui.buttonImport, &QPushButton::clicked, this, &SymbolManager::buttonImport);

	setTreeModel(); //设置树状模型

	connect(ui.buttonExport, &QPushButton::clicked, this, &SymbolManager::buttonExport);
}

SymbolManager::~SymbolManager()
{}

//更新左侧树状视图
void SymbolManager::setTreeModel() {
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui.treeView->model());
	model->clear();

	QStandardItem* allSymbols = new QStandardItem(QStringLiteral("全部"));
	allSymbols->setData("all");
	allSymbols->setEditable(false);
	QFont font = allSymbols->font();
	font.setBold(true);
	allSymbols->setFont(font);
	model->appendRow(allSymbols);

	QStandardItem* favoriteSymbols = new QStandardItem(QStringLiteral("收藏"));
	favoriteSymbols->setData("favorite");
	favoriteSymbols->setEditable(false);
	font = favoriteSymbols->font();
	font.setBold(true);
	favoriteSymbols->setFont(font);
	model->appendRow(favoriteSymbols);

	QStandardItem* taggroup = new QStandardItem(QString()); //可置空
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
	taggroup->setText(QStringLiteral("标签"));
	font = taggroup->font();
	font.setBold(true);
	taggroup->setFont(font);
	model->appendRow(taggroup);

	//扩展大小
	int rows = model->rowCount(model->indexFromItem(model->invisibleRootItem()));
	for (int i = 0; i < rows; i++)
	{
		ui.treeView->setExpanded(model->indexFromItem(model->item(i)), true);
	}
}

//更新右侧列表视图
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
	mModel->setEntityFilter(isSymbol ? QgsStyle::SymbolEntity : QgsStyle::ColorrampEntity);  // 设置实体过滤器
	if (type != 3)
	{
		mModel->setEntityFilterEnabled(true); // 符号类型过滤器
		mModel->setSymbolTypeFilterEnabled(true); // 符号类型过滤器
		mModel->setSymbolType(static_cast<QgsSymbol::SymbolType>(type)); // 设置符号类型

	}
	else
	{
		mModel->setEntityFilterEnabled(true);
		mModel->setSymbolTypeFilterEnabled(false);
	}

	QModelIndex index = ui.treeView->selectionModel()->currentIndex();
	groupChanged(index);
}

//更新右侧列表视图
void SymbolManager::groupChanged(const QModelIndex& index)
{
	QStringList groupSymbols;

	const QString category = index.data(Qt::UserRole + 1).toString();
	if (category == QLatin1String("all") || category == QLatin1String("tags"))
	{
		mModel->setTagId(-1); // 全部
		mModel->setSmartGroupId(-1);
		mModel->setFavoritesOnly(false);
	}
	else if (category == QLatin1String("favorite"))
	{
		mModel->setTagId(-1);
		mModel->setSmartGroupId(-1);
		mModel->setFavoritesOnly(true); // 收藏
	}
	else // 其他
	{
		int tagId = index.data(Qt::UserRole + 1).toInt();
		mModel->setTagId(tagId);
		mModel->setSmartGroupId(-1);
		mModel->setFavoritesOnly(false);
	}
}

//导入符号库
void SymbolManager::buttonImport() {
	bool isDelete = false;  //是否删除当前符号库
	if (mStyle->symbolCount() > 0) {
		int res = QMessageBox::warning(this, QStringLiteral("导入符号库"), QStringLiteral("是否需要清除当前符号库再导入？"), QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
		if (res == QMessageBox::StandardButton::Yes) { //清除符号库
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			isDelete = true;
			deleteSymbols(QgsStyle::StyleEntity::SymbolEntity, mStyle);
			deleteSymbols(QgsStyle::StyleEntity::TagEntity, mStyle);
			mStyle->clear();
			setTreeModel(); //设置树状模型
			tabItemTypeChanged();
			QApplication::restoreOverrideCursor(); //恢复鼠标样式
		}
	}

	QStringList fileNames = QFileDialog::getOpenFileNames(this, QStringLiteral("导入符号库"), "./res", QStringLiteral("XML 文件 (*.xml)"));
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
			QMessageBox::warning(this, QStringLiteral("导入符号库"), QStringLiteral("导入“%1”出错，请重试或者检查文件！").arg(file.baseName()));
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
	setTreeModel(); //设置树状模型
	//隐藏加载时间
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
	QMessageBox::warning(this, QStringLiteral("导入符号库"), QStringLiteral("导入完成！"));
}


//删除符号
bool SymbolManager::deleteSymbols(QgsStyle::StyleEntity type, QgsStyle* style) {
	QStringList names = style->allNames(type); //获取所有符号名称
	bool groupRemoved = false; //是否删除了组
	QString query; //删除语句
	switch (type) //根据类型生成删除语句
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
	int nErr = sqlite3_exec(mCurrentDB.get(), query.toUtf8().constData(), nullptr, nullptr, &zErr);//执行删除语句

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

//导出符号库
void SymbolManager::buttonExport() {
	if (!mSymbolExport) {
		mSymbolExport = new SymbolExport(this);
	}
	mSymbolExport->show();
}





