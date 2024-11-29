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
	void on_pushButtonInput_clicked();//ѡ������ʸ���ļ�
	void on_pushButtonOutput_clicked();//ѡ�������ַ
	void on_pushButtonOK_clicked();//���л���������

private:
	Ui::BufferClass ui;
	QGraphicsScene* scene;  // ������ graphicsView ����ʾ��
	//QVector<Point> points;  // �洢���еĵ�����
	QString inputFilePath;//�����ļ�·��
	QString outputFilePath;//����ļ���·��
	QString outputFileName;
	float bufferRadius;//�������뾶
};
