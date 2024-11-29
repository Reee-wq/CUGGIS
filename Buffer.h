#pragma once
#ifndef M_PI
#define M_PI 3.14159265
#endif
#include <QMainWindow>
#include "ui_Buffer.h"
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
#include <QGraphicsScene>

class Buffer : public QMainWindow
{
	Q_OBJECT

public:
	Buffer(QWidget* parent = nullptr);
	~Buffer();

private slots:
	void on_pushButtonInput_clicked();//选择输入矢量文件
	void on_pushButtonOutput_clicked();//选择输出地址
	void on_pushButtonOK_clicked();//进行缓冲区分析

private:
	Ui::BufferClass ui;
	QGraphicsScene* scene;  // 用于在 graphicsView 中显示点
	//QVector<Point> points;  // 存储所有的点数据
	QString inputFilePath;//输入文件路径
	QString outputFilePath;//输出文件夹路径
	QString outputFileName;
	float bufferRadius;//缓冲区半径
};
