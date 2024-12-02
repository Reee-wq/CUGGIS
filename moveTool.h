#pragma once
#ifndef MOVE_TOOL_H
#define MOVE_TOOL_H

#include <qgsmaptoolidentifyfeature.h>
#include <qgsmapcanvas.h>
#include <qgsfeature.h>
#include <qgsvectorlayer.h>
#include <QSet>
#include <QPointF>

class CustomMoveTool : public QgsMapToolIdentifyFeature {
    Q_OBJECT

public:
    explicit CustomMoveTool(QgsMapCanvas* canvas);  // 构造函数
    void canvasPressEvent(QgsMapMouseEvent* event) override;  // 鼠标按下事件
    void canvasReleaseEvent(QgsMapMouseEvent* event) override;  // 鼠标释放事件

signals:
    void featuresMoved(const QString& message);  // 发射移动成功的信号

private:
    QgsVectorLayer* getFirstVectorLayer();  // 获取第一个矢量图层
    QgsFeature mSelectedFeature;  // 当前选中的要素
    QgsPointXY mInitialMousePoint;  // 鼠标按下的初始位置
    QgsGeometry mOriginalGeometry;  // 要素的原始几何
    QList<QgsFeature> mSelectedFeatures;
    bool mDragging;  // 标志是否正在拖拽
    bool mCtrlPressed = false;  // 记录Ctrl键的状态
};

#endif // MOVE_TOOL_H
