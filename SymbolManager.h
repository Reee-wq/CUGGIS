#pragma once

#include <QMainWindow>

#include "StyleManager.h"
#include "SymbolExport.h"
#include <qgsGui.h>
#include<QAction>
#include<QStandardItemModel>
#include<QStandardItem>
#include<QFont>
#include<QCursor>
#include<QApplication>]
#include<QgsSettings.h>
#include<QFileDialog>

#include "ui_SymbolManager.h"
#include "qgsstylemodel.h"
#include "qgsstyle.h"
#include "QMessageBox"

class SymbolManager : public QMainWindow
{
	Q_OBJECT

public:
	struct ItemDetails
	{
		QgsStyle::StyleEntity entityType;
		QgsSymbol::SymbolType symbolType;
		QString name;
		bool isFavorite;
	};

	QgsStyle* getStyle();
	QgsStyleProxyModel* getModel();

	SymbolManager(QWidget* parent = nullptr);
	~SymbolManager();

private:
	Ui::SymbolManagerClass ui;

	void groupChanged(const QModelIndex&);      //左树界面：标签切换响应


	QgsStyle* mStyle = nullptr;
	QgsStyleProxyModel* mModel = nullptr;		 //左、中界面的model

	QAction* mActionExportSVG = nullptr;		 //导出事件响应
	QAction* mActionExportImage = nullptr;		 //导出事件响应

	QStringList mGetSymbolNames;
	QgsStyle* mGetStyle;
	std::unique_ptr< QgsStyle > mTempStyle;
	QgsStyle* mShowStyle;
	SymbolExport* mSymbolExport = nullptr;

	void setTreeModel();   //设置左树界面model
	void tabItemTypeChanged();                   //tab类型切换响应
	void buttonImport();                         //导入事件响应
	void buttonExport();                         //导出事件响应
	bool deleteSymbols(QgsStyle::StyleEntity type, QgsStyle* style); //删除事件响应

};
