#pragma once
#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include<QgsVectorLayer.h>
#include<QgsStyle.h>
#include "qgsstylemodel.h"
#include "StyleManager.h"
#include "ui_SymbolModify.h"
class SymbolModify : public QMainWindow
{
	Q_OBJECT

public:
	SymbolModify(QWidget* parent = nullptr, QgsMapLayer* layer = nullptr);
	
	~SymbolModify();

	QgsStyle* mStyle = nullptr;
	QgsStyleProxyModel* mModel = nullptr;
	QgsMapLayer* m_curMapLayer=nullptr;  //当前选择的图层
	void on_buttonModify_click();

private:
	Ui::SymbolModifyClass ui;
	


};
