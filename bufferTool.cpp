#include "bufferTool.h"
#include <qgsproject.h>
#include <qgsmapcanvas.h>
#include <qgsgeometry.h>
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include <qgsprocessingalgorithm.h>
#include <qgsapplication.h>
#include <qgsprocessingregistry.h>

// 构造函数
CustomBufferTool::CustomBufferTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), mIsFeatureSelected(false) {}

// 鼠标释放事件处理（左键选中要素，右键输入缓冲半径）
void CustomBufferTool::canvasReleaseEvent(QgsMapMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 获取第一个矢量图层
        QgsVectorLayer* layer = getFirstVectorLayer();
        if (!layer) {
            qDebug() << "No vector layers found!";
            return;
        }

        // 使用 identify 方法进行空间查询
        QList<QgsMapToolIdentify::IdentifyResult> results = identify(
            event->pos().x(), event->pos().y(),
            QList<QgsMapLayer*>{layer},
            QgsMapToolIdentify::TopDownStopAtFirst
        );

        if (!results.isEmpty()) {
            // 获取第一个匹配的要素
            mSelectedFeature = results.first().mFeature;
            mIsFeatureSelected = true;

            // 调试输出
            qDebug() << "Selected Feature ID:" << mSelectedFeature.id();

            // 选中要素
            layer->select(mSelectedFeature.id());
        }
        else {
            qDebug() << "No feature selected!";
            mIsFeatureSelected = false;
        }
    }
    else if (event->button() == Qt::RightButton) {
        if (mIsFeatureSelected) {
            // 弹出对话框让用户输入缓冲半径
            bool ok;
            double radius = QInputDialog::getDouble(
                nullptr,
                "Input Buffer Radius",
                "Enter buffer radius:",
                0.0,        // 默认值
                0.0,        // 最小值
                10000.0,    // 最大值
                5,          // 小数点位数
                &ok
            );

            if (ok) {
                // 执行缓冲区生成
                QgsVectorLayer* layer = getFirstVectorLayer();
                if (layer) {
                    createBuffer(layer, mSelectedFeature, radius);
                }
            }
            else {
                qDebug() << "Buffer creation cancelled.";
            }
        }
        else {
            qDebug() << "No feature selected for buffering!";
        }
    }
}

QgsVectorLayer* CustomBufferTool::getFirstVectorLayer() {
    auto layers = QgsProject::instance()->mapLayers().values();
    for (QgsMapLayer* layer : layers) { // 确保正确类型
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer;
        }
    }
    return nullptr;
}

void CustomBufferTool::createBuffer(QgsVectorLayer* layer, QgsFeature& feature, double radius) {
    QgsGeometry geometry = feature.geometry();

    if (!geometry.isNull() && geometry.isGeosValid()) {
        // 创建缓冲区
        QgsGeometry bufferGeometry = geometry.buffer(radius, 5);  // 半径和缓冲区的分段数

        if (!bufferGeometry.isNull() && bufferGeometry.isGeosValid()) {
            // 创建一个新的要素来存储缓冲区几何
            QgsFeature bufferFeature;
            bufferFeature.setGeometry(bufferGeometry);

            // 创建一个内存图层来存储缓冲区
            QgsVectorLayer* bufferLayer = new QgsVectorLayer("Polygon?crs=" + layer->crs().toWkt(), "Buffer Layer", "memory");
            if (bufferLayer->isValid()) {
                QgsVectorDataProvider* provider = bufferLayer->dataProvider();

                // 创建要素列表并加入缓冲区要素
                QList<QgsFeature> features;
                features.append(bufferFeature);

                // 将缓冲区要素列表添加到图层
                provider->addFeatures(features);

                // 将缓冲区图层添加到项目中
                QgsProject::instance()->addMapLayer(bufferLayer);
                qDebug() << "缓冲区创建成功！";
            }
            else {
                QMessageBox::critical(nullptr, "错误", "无法创建缓冲区图层！");
            }
        }
        else {
            QMessageBox::critical(nullptr, "错误", "缓冲区几何无效！");
        }
    }
    else {
        QMessageBox::critical(nullptr, "错误", "选中的要素无效！");
    }
}
