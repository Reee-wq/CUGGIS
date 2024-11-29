#pragma once
#ifndef M_PI
#define M_PI 3.14159265
#endif
#include <QMainWindow>
#include "ui_SHPtoGRID.h"
#include <QPushButton>
#include <QMessageBox>
#include <QWidget>
#include <qgis.h>
#include <qgsvectorlayer.h>
#include <qgsproject.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <QgsVectorLayer.h>
#include <QgsProcessingFeedback.h>
#include <QgsProcessingAlgorithm.h>
#include <qgsvectorlayer.h>
#include <qgsvectorfilewriter.h>
#include "qgsrasterlayer.h"
#include "qgsproject.h"


class SHPtoGRID : public QMainWindow
{
	Q_OBJECT

public:
	SHPtoGRID(QWidget* parent = nullptr);
	~SHPtoGRID();
	//void rasterizeWithGDAL(const QString& inputFilePath, const QString& outputFilePath);
	//QString rasterOutputPath;

//signals:
//	void rasterGenerated(const QString& rasterFilePath); // 信号：传递栅格文件路径


private slots:
	void on_pushButtonInput_clicked();//选择输入矢量文件
	void on_pushButtonOutput_clicked();//选择输出地址
	void on_pushButtonOK_clicked();//点击进行转换

private:
	Ui::SHPtoGRIDClass ui;

	QString inputFilePath;//输入文件路径
	QString outputFilePath;//输出文件夹路径
	QString outputFileName;//输出文件名
};