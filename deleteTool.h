#pragma once
#ifndef DELETETOOL_H
#define DELETETOOL_H

#include <qgsmaptoolidentifyfeature.h>
#include <qgsmapcanvas.h>
#include <qgsfeature.h>
#include <qgsvectorlayer.h>
#include <QSet>

class CustomDeleteTool : public QgsMapToolIdentifyFeature {
    Q_OBJECT

public:
    explicit CustomDeleteTool(QgsMapCanvas* canvas);  // 构造函数
    void canvasReleaseEvent(QgsMapMouseEvent* event) override;  // 鼠标释放事件
    void keyPressEvent(QKeyEvent* event) override;  // 键盘按下事件
    void keyReleaseEvent(QKeyEvent* event) override;  // 键盘释放事件

signals:
    void featuresDeleted(const QString& message);  // 发射删除成功的信号

private:
    QgsVectorLayer* getFirstVectorLayer();  // 获取第一个矢量图层
    void deleteFeatures(QgsVectorLayer* layer, const QList<QgsFeatureId>& featureIds);  // 删除选中的要素

    bool mCtrlPressed;  // 标记 Ctrl 键是否按下
    QSet<QgsFeatureId> mSelectedFeatureIds;  // 存储选中的要素 ID
};

#endif // DELETETOOL_H
