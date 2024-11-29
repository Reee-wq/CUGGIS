#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_SymbolExport.h"
#include "qgsstylemodel.h"
#include "StyleManager.h"

class SymbolExport : public QMainWindow
{
	Q_OBJECT

public:
	SymbolExport(QWidget* parent = nullptr);
	~SymbolExport();


private:
	Ui::SymbolExportClass ui;
	QgsStyle* mStyle = nullptr;
	QgsStyleProxyModel* mModel = nullptr;

	void onbuttonExportClicked();
};
