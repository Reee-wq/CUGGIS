#ifndef BUFFER_TOOL_H
#define BUFFER_TOOL_H

#include <qgsmaptoolidentifyfeature.h>
#include <qgsvectorlayer.h>
#include <qgsgeometry.h>
#include <qgsfeature.h>
#include <qgsmapmouseevent.h>
#include <QObject>

class CustomBufferTool : public QgsMapToolIdentifyFeature {
    Q_OBJECT

public:
    explicit CustomBufferTool(QgsMapCanvas* canvas);

    void canvasReleaseEvent(QgsMapMouseEvent* event) override;

private:
    QgsVectorLayer* getFirstVectorLayer();       // 获取第一个矢量图层
    void createBuffer(QgsVectorLayer* layer, QgsFeature& feature, double radius); // 执行缓冲区计算

    QgsFeature mSelectedFeature;                // 被选中的要素
    bool mIsFeatureSelected = false;            // 是否选中要素
};

#endif // BUFFER_TOOL_H
