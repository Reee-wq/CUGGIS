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
    explicit CustomMoveTool(QgsMapCanvas* canvas);  // ���캯��
    void canvasPressEvent(QgsMapMouseEvent* event) override;  // ��갴���¼�
    void canvasReleaseEvent(QgsMapMouseEvent* event) override;  // ����ͷ��¼�

signals:
    void featuresMoved(const QString& message);  // �����ƶ��ɹ����ź�

private:
    QgsVectorLayer* getFirstVectorLayer();  // ��ȡ��һ��ʸ��ͼ��
    QgsFeature mSelectedFeature;  // ��ǰѡ�е�Ҫ��
    QgsPointXY mInitialMousePoint;  // ��갴�µĳ�ʼλ��
    QgsGeometry mOriginalGeometry;  // Ҫ�ص�ԭʼ����
    QList<QgsFeature> mSelectedFeatures;
    bool mDragging;  // ��־�Ƿ�������ק
    bool mCtrlPressed = false;  // ��¼Ctrl����״̬
};

#endif // MOVE_TOOL_H
