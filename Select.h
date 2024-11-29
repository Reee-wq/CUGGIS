#pragma once
#include <qgsmaptoolidentifyfeature.h>
#include <qgsmapcanvas.h>
#include <qgsfeature.h>
#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <qgsvectorlayerselectionmanager.h> // 添加这个头文件

// 自定义识别工具类
class CustomIdentifyTool : public QgsMapToolIdentifyFeature {
    Q_OBJECT
public:
    explicit CustomIdentifyTool(QgsMapCanvas* canvas);  // 只需要传入地图画布
    void canvasReleaseEvent(QgsMapMouseEvent* event) override;

private slots:
    void showAttributeEditor();  // 显示属性编辑器


private:
    QgsVectorLayer* getFirstVectorLayer();  // 获取第一个矢量图层
    void showAttributeEditor(QgsFeature feature);  // 显示属性编辑器
    QMenu* m_contextMenu;  // 右键菜单
    QgsVectorLayer* m_currentLayer;  // 当前矢量图层
    QgsFeature m_currentFeature;  // 当前选中的要素
};
