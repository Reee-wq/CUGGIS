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

    // ��ȡ��һ��ʸ��ͼ��
    QgsVectorLayer* getFirstVectorLayer();

protected:
    // ��д����¼�������������Ҽ����
    void canvasPressEvent(QgsMapMouseEvent* event) override;

    // ��д�����¼�������Ctrl������
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    QgsMapCanvas* mCanvas;  // ��ͼ����
    QgsFeature mSelectedFeature;  // ��ǰѡ�е�Ҫ��
    bool mCtrlPressed;  // �Ƿ���Ctrl��
};

#endif // PARALLEL_H
