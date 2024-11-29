/**************************ѡ��ͼ��Ҫ�ع���*********************

*****************************************************************/
#pragma execution_character_set("utf-8")
#ifndef QGSMAPTOOLSELECT_H
#define QGSMAPTOOLSELECT_H
#pragma once
//Qgisͷ�ļ�
#include <qgsmaptool.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsrubberband.h>
#include <QgsVectorFileWriter.h>


//Qtͷ�ļ�
#include <QgsMapMouseEvent.h>
#include <QPoint>
#include <QRect>
#include <QMessageBox>
#include <QApplication>
#include <limits>
#include <QKeyEvent>
#include <QFileDialog>
#include <QVariant>

#include <gdal.h>
#include <ogr_api.h>
//#include <ogr_geometry.h>
#include <ogr_feature.h>
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>


class QgsMapToolSelect : public QgsMapTool
{
	Q_OBJECT;
public:
	QgsMapToolSelect(QgsMapCanvas*);
	~QgsMapToolSelect(void);
public:

	//���õ�ǰ��ѡ��(�)��ͼ��
	void SetSelectLayer(QgsVectorLayer*);

	//��������¼�����
	virtual void canvasReleaseEvent(QgsMapMouseEvent* e);
	virtual void canvasPressEvent(QgsMapMouseEvent* e);
	virtual void canvasMoveEvent(QgsMapMouseEvent* e);

	//������ʾ
	void highlightSelectedFeatures();

	//�趨����״̬
	void SetEnable(bool);
	void keyPressEvent(QKeyEvent* e);
	void deleteSelectedFeatures();

private:
	QgsVectorLayer* pLayer;
	QgsFeatureIds layerSelectedFeatures;
	bool StatusFlag;

	QgsPointXY startPoint_;
	QgsPointXY endPoint_;
	QgsRubberBand* rubberBand_ = nullptr;
	//QgsRubberBand* rubberBand_ = new  QgsRubberBand(mCanvas);
	bool selecting_ = false;
	QgsRubberBand* highlightRubberBand_ = nullptr;
	QSet<QgsFeatureId> selectedFeatures_;

private:
	//��ȡ���λ��һ����Χ��Ϊѡ������
	void ExpandSelectRangle(QRect& Rect, QPoint Point);
	//��ָ�����豸��������ת���ɵ�ͼ��������
	void SetRubberBand(QRect& selectRect, QgsRubberBand*);

	QgsMapToolSelect* selectTool;  // ���ڴ洢ѡ�񹤾�ʵ��

	QTimer* blinkTimer_ = nullptr;  // ���ڿ��Ƹ�����˸�Ķ�ʱ��
};
#endif