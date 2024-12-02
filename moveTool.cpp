#include "moveTool.h"
#include <qgsproject.h>
#include <qgsgeometry.h>
#include <QDebug>
#include <QMessageBox>
#include<qgsmapmouseevent.h>

// 构造函数
CustomMoveTool::CustomMoveTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), mDragging(false) {}

// 获取第一个矢量图层
QgsVectorLayer* CustomMoveTool::getFirstVectorLayer() {
    auto layers = QgsProject::instance()->mapLayers().values();
    for (auto layer : layers) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer;  // 返回找到的第一个矢量图层
        }
    }
    return nullptr;  // 如果没有矢量图层，返回空指针
}

void CustomMoveTool::canvasPressEvent(QgsMapMouseEvent* event) {
    // 获取第一个矢量图层
    QgsVectorLayer* layer = getFirstVectorLayer();
    if (!layer) {
        qDebug() << "No vector layers found!";
        return;
    }

    // 获取鼠标点击的屏幕坐标
    int x = event->pos().x();
    int y = event->pos().y();

    // 检查是否按下Ctrl键
    mCtrlPressed = event->modifiers() & Qt::ControlModifier;

    if (mCtrlPressed) {
        // 按下Ctrl键，进入选择模式
        QList<QgsMapToolIdentify::IdentifyResult> results = identify(
            x, y,
            QList<QgsMapLayer*>{layer},
            QgsMapToolIdentify::TopDownStopAtFirst
        );

        if (!results.isEmpty()) {
            QgsFeature feature = results.first().mFeature;
            if (!mSelectedFeatures.contains(feature)) {
                mSelectedFeatures.append(feature);  // 加入选中列表
                qDebug() << "Feature added to selection with ID:" << feature.id();
            }
        }
    }
    else if (!mSelectedFeatures.isEmpty()) {
        // 松开Ctrl键后，再次点击开始记录偏移终点
        mInitialMousePoint = toMapCoordinates(event->pos());
        mDragging = true;
        qDebug() << "Moving selected features. Initial position recorded.";
    }
}

void CustomMoveTool::canvasReleaseEvent(QgsMapMouseEvent* event) {
    QgsVectorLayer* layer = getFirstVectorLayer();
    if (!layer) {
        qDebug() << "No vector layers found!";
        return;
    }

    if (!mDragging || mSelectedFeatures.isEmpty()) {
        qDebug() << "Nothing to move.";
        return;
    }

    // 获取鼠标释放时的位置
    QgsPointXY releaseMousePoint = toMapCoordinates(event->pos());

    // 计算第一个要素的偏移量
    QgsFeature firstFeature = mSelectedFeatures.first();
    QgsGeometry firstGeometry = firstFeature.geometry();
    double deltaX = releaseMousePoint.x() - firstGeometry.centroid().asPoint().x();
    double deltaY = releaseMousePoint.y() - firstGeometry.centroid().asPoint().y();

    // 确保图层处于编辑状态
    if (!layer->isEditable()) {
        layer->startEditing();
    }

    // 更新所有选中要素的几何体
    for (QgsFeature& feature : mSelectedFeatures) {
        QgsGeometry geometry = feature.geometry();
        geometry.translate(deltaX, deltaY);
        layer->changeGeometry(feature.id(), geometry);
    }

    // 提交修改
    if (!layer->commitChanges()) {
        qDebug() << "Failed to save changes to the layer.";
        layer->rollBack();
    }
    else {
        qDebug() << "Selected features moved successfully.";
        emit featuresMoved("Selected features moved successfully.");
    }

    // 清空选中的要素列表并重置拖拽状态
    mSelectedFeatures.clear();
    mDragging = false;
}
