#include "parallelTool.h"
#include <QgsMapToolIdentify.h>
#include <QgsGeometry.h>
#include <QDebug>
#include <QgsProject.h>
#include <QgsFeature.h>
#include <QgsVectorLayer.h>
#include <QgsMapCanvas.h>
#include <QgsMapMouseEvent.h>
#include <QInputDialog>

CustomParallelTool::CustomParallelTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), mCanvas(canvas), mSelectedFeature(QgsFeature()), mCtrlPressed(false)
{
    setCursor(Qt::CrossCursor);  // 设置十字光标
    qDebug() << "CustomParallelTool activated.";
}

CustomParallelTool::~CustomParallelTool()
{
    // 析构函数
}

QgsVectorLayer* CustomParallelTool::getFirstVectorLayer() {
    auto layers = QgsProject::instance()->mapLayers().values();
    for (auto layer : layers) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer;  // 返回找到的第一个矢量图层
        }
    }
    return nullptr;  // 如果没有矢量图层，返回空指针
}

void CustomParallelTool::canvasPressEvent(QgsMapMouseEvent* event)
{
    // 获取第一个矢量图层
    QgsVectorLayer* layer = getFirstVectorLayer();
    if (!layer) {
        qDebug() << "No vector layers found!";
        return;
    }

    // 如果是左键点击，选择要素
    if (event->button() == Qt::LeftButton) {
        QList<QgsMapToolIdentify::IdentifyResult> results = identify(
            event->pos().x(), event->pos().y(),               // 屏幕坐标
            QList<QgsMapLayer*>{layer},           // 指定查询的图层
            QgsMapToolIdentify::TopDownStopAtFirst // 查询模式：找到第一个匹配的要素
        );

        if (!results.isEmpty()) {
            mSelectedFeature = results.first().mFeature;
            qDebug() << "Selected feature ID:" << mSelectedFeature.id();

            // 选中要素
            layer->select(mSelectedFeature.id());

            // 输出要素属性（可选）
            auto attributes = mSelectedFeature.attributes();
            qDebug() << "Attributes:" << attributes;
        }
        else {
            qDebug() << "No feature found at clicked position!";
            mSelectedFeature = QgsFeature(); // 重置已选要素
        }
    }

    // 如果是右键点击，并且 Ctrl 被按下，执行复制逻辑
    if (event->button() == Qt::RightButton && mCtrlPressed) {
        mCtrlPressed = false; // 重置标记，避免重复触发

        // 确保选择了有效要素
        if (mSelectedFeature.geometry().isNull()) {
            qDebug() << "No feature selected for copying!";
            return;
        }

        // 弹出对话框让用户输入平行线的间隔
        bool ok;
        double interval = QInputDialog::getDouble(
            nullptr,
            "Input Parallel Line Spacing",
            "Enter parallel line spacing (positive for right, negative for left):",
            0.1,        // 默认值
            -10000.0,   // 最小值
            10000.0,    // 最大值
            2,          // 小数点位数
            &ok
        );

        if (!ok) {
            qDebug() << "User canceled the input!";
            return;
        }

        // 确保图层处于编辑模式
        if (!layer->isEditable()) {
            if (!layer->startEditing()) {
                qDebug() << "Failed to start editing mode for the layer.";
                return;
            }
        }

        // 获取右键点击位置，作为复制的基准位置
        QgsPointXY basePos = mCanvas->getCoordinateTransform()->toMapCoordinates(event->pos());

        // 获取要素的几何体
        QgsGeometry geometry = mSelectedFeature.geometry();
        QgsGeometry originalGeometry = QgsGeometry::fromWkt(geometry.asWkt()); // 深拷贝几何体

        // 计算原始要素的中心点
        QgsPointXY centroid = originalGeometry.centroid().asPoint();

        // 计算右侧的平行线几何体
        QgsGeometry rightGeometry = QgsGeometry::fromWkt(originalGeometry.asWkt());
        rightGeometry.translate(interval, 0); // 向右平移
        QgsFeature rightFeature = mSelectedFeature;
        rightFeature.setGeometry(rightGeometry);

        // 计算左侧的平行线几何体
        QgsGeometry leftGeometry = QgsGeometry::fromWkt(originalGeometry.asWkt());
        leftGeometry.translate(-interval, 0); // 向左平移
        QgsFeature leftFeature = mSelectedFeature;
        leftFeature.setGeometry(leftGeometry);

        // 将平行线添加到图层
        if (!layer->addFeature(leftFeature)) {
            qDebug() << "Failed to add left parallel line feature!";
            return;
        }
        qDebug() << "Left parallel line added with ID:" << leftFeature.id();

        if (!layer->addFeature(rightFeature)) {
            qDebug() << "Failed to add right parallel line feature!";
            return;
        }
        qDebug() << "Right parallel line added with ID:" << rightFeature.id();

        // 提交更改并刷新地图画布
        if (layer->commitChanges()) {
            mCanvas->refresh();
            qDebug() << "Parallel lines copied successfully.";
        }
        else {
            qDebug() << "Failed to commit changes.";
            layer->rollBack();
        }
    }
}

void CustomParallelTool::keyPressEvent(QKeyEvent* event) {
    // 按下 Ctrl，标记进入复制模式
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = true;
        qDebug() << "Ctrl pressed. Ready for copying.";
    }
}

void CustomParallelTool::keyReleaseEvent(QKeyEvent* event) {
    // 松开 Ctrl 时重置标记
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = false;
        qDebug() << "Ctrl released.";
    }
}
