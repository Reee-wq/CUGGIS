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
    QgsVectorLayer* getFirstVectorLayer();       // ��ȡ��һ��ʸ��ͼ��
    void createBuffer(QgsVectorLayer* layer, QgsFeature& feature, double radius); // ִ�л���������

    QgsFeature mSelectedFeature;                // ��ѡ�е�Ҫ��
    bool mIsFeatureSelected = false;            // �Ƿ�ѡ��Ҫ��
};

#endif // BUFFER_TOOL_H
