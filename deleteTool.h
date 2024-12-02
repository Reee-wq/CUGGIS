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
    explicit CustomDeleteTool(QgsMapCanvas* canvas);  // ���캯��
    void canvasReleaseEvent(QgsMapMouseEvent* event) override;  // ����ͷ��¼�
    void keyPressEvent(QKeyEvent* event) override;  // ���̰����¼�
    void keyReleaseEvent(QKeyEvent* event) override;  // �����ͷ��¼�

signals:
    void featuresDeleted(const QString& message);  // ����ɾ���ɹ����ź�

private:
    QgsVectorLayer* getFirstVectorLayer();  // ��ȡ��һ��ʸ��ͼ��
    void deleteFeatures(QgsVectorLayer* layer, const QList<QgsFeatureId>& featureIds);  // ɾ��ѡ�е�Ҫ��

    bool mCtrlPressed;  // ��� Ctrl ���Ƿ���
    QSet<QgsFeatureId> mSelectedFeatureIds;  // �洢ѡ�е�Ҫ�� ID
};

#endif // DELETETOOL_H
