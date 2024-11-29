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

	void groupChanged(const QModelIndex&);      //�������棺��ǩ�л���Ӧ


	QgsStyle* mStyle = nullptr;
	QgsStyleProxyModel* mModel = nullptr;		 //���н����model

	QAction* mActionExportSVG = nullptr;		 //�����¼���Ӧ
	QAction* mActionExportImage = nullptr;		 //�����¼���Ӧ

	QStringList mGetSymbolNames;
	QgsStyle* mGetStyle;
	std::unique_ptr< QgsStyle > mTempStyle;
	QgsStyle* mShowStyle;
	SymbolExport* mSymbolExport = nullptr;

	void setTreeModel();   //������������model
	void tabItemTypeChanged();                   //tab�����л���Ӧ
	void buttonImport();                         //�����¼���Ӧ
	void buttonExport();                         //�����¼���Ӧ
	bool deleteSymbols(QgsStyle::StyleEntity type, QgsStyle* style); //ɾ���¼���Ӧ

};
