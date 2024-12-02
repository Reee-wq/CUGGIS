#pragma once
#ifndef PARALLEL_H
#define PARALLEL_H

#include <QgsMapCanvas.h>
#include <QgsMapToolIdentifyFeature.h>
#include <QgsFeature.h>
#include <QgsVectorLayer.h>
#include <QgsPointXY.h>
#include <QKeyEvent>

class CustomParallelTool : public QgsMapToolIdentifyFeature
{
    Q_OBJECT

public:
    explicit CustomParallelTool(QgsMapCanvas* canvas);
    ~CustomParallelTool();

    // 获取第一个矢量图层
    QgsVectorLayer* getFirstVectorLayer();

protected:
    // 重写鼠标事件，处理左键和右键点击
    void canvasPressEvent(QgsMapMouseEvent* event) override;

    // 重写键盘事件，处理Ctrl键按下
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    QgsMapCanvas* mCanvas;  // 地图画布
    QgsFeature mSelectedFeature;  // 当前选中的要素
    bool mCtrlPressed;  // 是否按下Ctrl键
};

#endif // PARALLEL_H
