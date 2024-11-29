
#pragma execution_character_set("utf-8")
#ifndef CIRCUTCUT_H
#define CIRCUTCUT_H
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
#include <ogr_geometry.h>
#include <ogr_feature.h>
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>


class CircleCut : public QgsMapTool
{
    Q_OBJECT;
public:
    CircleCut(QgsMapCanvas*);
    ~CircleCut(void);
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
    bool selecting_ = false;
    QgsRubberBand* highlightRubberBand_ = nullptr;
    QSet<QgsFeatureId> selectedFeatures_;

private:
    //绘制圆形选择区域
    void SetRubberBand(double startX, double startY, double endX, double endY, QgsRubberBand* rubberBand);

    CircleCut* selectTool;  // 用于存储选择工具实例

    QTimer* blinkTimer_ = nullptr;  // 用于控制高亮闪烁的定时器
};

#endif
