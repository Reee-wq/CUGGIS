#pragma once
#pragma execution_character_set("utf-8")

#include <QMainWindow>
#include <QgsMapLayer.h>
#include<vector>
#include<QgsStyle.h>
#include <QgsRasterLayer.h>
#include <QMessageBox>
#include<QgsSymbolLayerUtils.h>
#include <qgis.h>
#include<QgsColorRamp.h>
#include< QgsRasterShader.h>
#include< QgsRasterRenderer.h>
#include<QgsSingleBandPseudoColorRenderer.h>
#include< QStandardItemModel>
#include"StyleManager.h"
#include "qgsstylemodel.h"

#include "ui_Layering.h"

using namespace std;

class Layering : public QMainWindow
{
	Q_OBJECT

public:
	Layering(QWidget* parent = nullptr, QgsMapLayer* layer = nullptr);
	~Layering();

	QgsMapLayer* m_curMapLayer = nullptr;  //当前选择的图层
	QgsStyle* mStyle = nullptr; 
	QgsStyleProxyModel* mModel = nullptr;



private:
	Ui::LayeringClass ui;

	void on_buttonConfirm_click();
	void on_buttonCalculate_click();

	vector<QColor> colors;  // 分层颜色
	vector<double> breaks;  // 存储分割点

	int numClasses;   // 分层数目
	bool isCalculated = false;  // 是否计算过

	QgsColorRamp* colorRamp=nullptr;  // 颜色渐变

};
