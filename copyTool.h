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
    QgsMapCanvas* mCanvas;                       // 地图画布指针
    QList<QgsFeature> mSelectedFeatures;         // 选中的要素列表
    bool mCtrlPressed;                           // 是否按下了 Ctrl 键
    QList<QgsHighlight*> mHighlights;

    QgsVectorLayer* getFirstVectorLayer();       // 获取第一个矢量图层
    bool getUserInput(double& interval, int& copyCount); // 获取用户输入的参数
    void highlightSelectedFeatures(QgsVectorLayer* layer); // 高亮显示选中的要素
};

#endif // COPYTOOL_H
