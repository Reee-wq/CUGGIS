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
	void loadExcelFields();//������������ݣ�ѡ��X,Y����
private:
	void selectPath();//ѡ���ļ�·��
	void outPath();//ѡ������ļ���·��  ��+


private slots:
	void on_pushButtonOK_clicked();//OK����¼�
	void comboBoxX(int index);//X�ֶ�
	void comboBoxY(int index);//Y�ֶ�

private:
	Ui::readExcelClass ui;
	QgsMapCanvas* m_mapCanvas;
	QString excelFilePath;//�ļ�·��
	QString outPutFilePath;//������ļ�·��   ��+
	QString layerName;
};
