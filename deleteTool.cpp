#include "deleteTool.h"
#include <qgsproject.h>
#include <qgsgeometry.h>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include<qgsmapmouseevent.h>

// 构造函数
CustomDeleteTool::CustomDeleteTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), mCtrlPressed(false) {}

// 鼠标释放事件处理
void CustomDeleteTool::canvasReleaseEvent(QgsMapMouseEvent* event) {
    // 获取第一个矢量图层
    QgsVectorLayer* layer = getFirstVectorLayer();
    if (!layer) {
        qDebug() << "No vector layers found!";
        return;
    }

    // 获取鼠标点击的屏幕坐标
    int x = event->pos().x();
    int y = event->pos().y();

    // 使用 identify 方法进行空间查询
    QList<QgsMapToolIdentify::IdentifyResult> results = identify(
        x, y,                                // 屏幕坐标
        QList<QgsMapLayer*>{layer},          // 指定查询的图层
        QgsMapToolIdentify::TopDownStopAtFirst // 查询模式：找到第一个匹配的要素
    );

    if (!results.isEmpty()) {
        // 获取第一个匹配的要素
        QgsFeature feature = results.first().mFeature;

        if (mCtrlPressed) {
            // 如果 Ctrl 键按下，则添加到选中集合中
            mSelectedFeatureIds.insert(feature.id());
            qDebug() << "Added feature ID" << feature.id() << "to selection.";
        }
        else {
            // 如果没有按下 Ctrl 键，则只处理当前点击的要素
            mSelectedFeatureIds.clear();
            mSelectedFeatureIds.insert(feature.id());

            // 执行删除
            deleteFeatures(layer, QList<QgsFeatureId>{feature.id()});
        }
    }
    else {
        qDebug() << "No feature selected!";
    }
}

// 获取第一个矢量图层
QgsVectorLayer* CustomDeleteTool::getFirstVectorLayer() {
    auto layers = QgsProject::instance()->mapLayers().values();
    for (auto layer : layers) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer;  // 返回找到的第一个矢量图层
        }
    }
    return nullptr;  // 如果没有矢量图层，返回空指针
}

// 删除选中的要素
void CustomDeleteTool::deleteFeatures(QgsVectorLayer* layer, const QList<QgsFeatureId>& featureIds) {
    if (!layer->isEditable()) {
        layer->startEditing();  // 开启编辑模式
    }

    bool success = true;
    for (auto featureId : featureIds) {
        if (!layer->deleteFeature(featureId)) {
            qDebug() << "Failed to delete feature with ID" << featureId;
            success = false;
        }
        else {
            qDebug() << "Feature with ID" << featureId << "deleted.";
        }
    }

    // 提交编辑
    if (success && layer->commitChanges()) {
        qDebug() << "Changes committed successfully.";
        emit featuresDeleted(QString("Successfully deleted %1 feature(s).").arg(featureIds.size()));
    }
    else {
        qDebug() << "Failed to commit changes to the layer.";
        layer->rollBack();  // 如果提交失败，回滚编辑
    }

    // 清空选中集合
    mSelectedFeatureIds.clear();
}

// 键盘按下事件处理
void CustomDeleteTool::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = true;
        qDebug() << "Ctrl key pressed.";
    }
}

// 键盘释放事件处理
void CustomDeleteTool::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = false;
        qDebug() << "Ctrl key released.";

        // 批量删除已选中的要素
        QgsVectorLayer* layer = getFirstVectorLayer();
        if (layer && !mSelectedFeatureIds.isEmpty()) {
            deleteFeatures(layer, mSelectedFeatureIds.values());
        }
    }
}
