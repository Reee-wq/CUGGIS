#pragma once
//#pragma execution_character_set("utf-8")
#include <QDialog>
#include <QMainWindow>
#include "ui_readExcel.h"
#include <QgsMapCanvas.h>


class readExcel : public QMainWindow
{
	Q_OBJECT

public:
	explicit readExcel(QgsMapCanvas* mapCanvas,QWidget *parent = nullptr);
	~readExcel();
	void loadExcelFields();//添加下拉框内容，选择X,Y坐标
private:
	void selectPath();//选择文件路径
	void outPath();//选择输出文件的路径  新+


private slots:
	void on_pushButtonOK_clicked();//OK点击事件
	void comboBoxX(int index);//X字段
	void comboBoxY(int index);//Y字段

private:
	Ui::readExcelClass ui;
	QgsMapCanvas* m_mapCanvas;
	QString excelFilePath;//文件路径
	QString outPutFilePath;//输出的文件路径   新+
	QString layerName;
};
