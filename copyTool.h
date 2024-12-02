#ifndef COPYTOOL_H
#define COPYTOOL_H

#include <QgsMapToolIdentifyFeature.h>
#include <QKeyEvent>
#include <QgsFeature.h>
#include <QList>
#include<qgsmapcanvas.h>
#include<qgsvectorlayer.h>


class CustomCopyTool : public QgsMapToolIdentifyFeature
{
    Q_OBJECT

public:
    explicit CustomCopyTool(QgsMapCanvas* canvas);
    ~CustomCopyTool() override;

    void canvasPressEvent(QgsMapMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    QgsMapCanvas* mCanvas;                       // ��ͼ����ָ��
    QList<QgsFeature> mSelectedFeatures;         // ѡ�е�Ҫ���б�
    bool mCtrlPressed;                           // �Ƿ����� Ctrl ��
    QList<QgsHighlight*> mHighlights;

    QgsVectorLayer* getFirstVectorLayer();       // ��ȡ��һ��ʸ��ͼ��
    bool getUserInput(double& interval, int& copyCount); // ��ȡ�û�����Ĳ���
    void highlightSelectedFeatures(QgsVectorLayer* layer); // ������ʾѡ�е�Ҫ��
};

#endif // COPYTOOL_H
