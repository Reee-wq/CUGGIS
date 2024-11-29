/**************************选择图层要素工具*********************

*****************************************************************/
#pragma execution_character_set("utf-8")
#ifndef QGSMAPTOOLSELECT_H
#define QGSMAPTOOLSELECT_H
#pragma once
//Qgis头文件
#include <qgsmaptool.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsrubberband.h>
#include <QgsVectorFileWriter.h>


//Qt头文件
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

	//设置当前被选择(活动)的图层
	void SetSelectLayer(QgsVectorLayer*);

	//重载鼠标事件函数
	virtual void canvasReleaseEvent(QgsMapMouseEvent* e);
	virtual void canvasPressEvent(QgsMapMouseEvent* e);
	virtual void canvasMoveEvent(QgsMapMouseEvent* e);

	//高亮显示
	void highlightSelectedFeatures();

	//设定工具状态
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
	//提取鼠标位置一定范围作为选择区域
	void ExpandSelectRangle(QRect& Rect, QPoint Point);
	//将指定的设备坐标区域转换成地图坐标区域
	void SetRubberBand(QRect& selectRect, QgsRubberBand*);

	QgsMapToolSelect* selectTool;  // 用于存储选择工具实例

	QTimer* blinkTimer_ = nullptr;  // 用于控制高亮闪烁的定时器
};
#endif