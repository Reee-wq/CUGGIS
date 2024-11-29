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

	QgsMapLayer* m_curMapLayer = nullptr;  //��ǰѡ���ͼ��
	QgsStyle* mStyle = nullptr; 
	QgsStyleProxyModel* mModel = nullptr;



private:
	Ui::LayeringClass ui;

	void on_buttonConfirm_click();
	void on_buttonCalculate_click();

	vector<QColor> colors;  // �ֲ���ɫ
	vector<double> breaks;  // �洢�ָ��

	int numClasses;   // �ֲ���Ŀ
	bool isCalculated = false;  // �Ƿ�����

	QgsColorRamp* colorRamp=nullptr;  // ��ɫ����

};
