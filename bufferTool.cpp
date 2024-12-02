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

// ���캯��
CustomBufferTool::CustomBufferTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), mIsFeatureSelected(false) {}

// ����ͷ��¼��������ѡ��Ҫ�أ��Ҽ����뻺��뾶��
void CustomBufferTool::canvasReleaseEvent(QgsMapMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // ��ȡ��һ��ʸ��ͼ��
        QgsVectorLayer* layer = getFirstVectorLayer();
        if (!layer) {
            qDebug() << "No vector layers found!";
            return;
        }

        // ʹ�� identify �������пռ��ѯ
        QList<QgsMapToolIdentify::IdentifyResult> results = identify(
            event->pos().x(), event->pos().y(),
            QList<QgsMapLayer*>{layer},
            QgsMapToolIdentify::TopDownStopAtFirst
        );

        if (!results.isEmpty()) {
            // ��ȡ��һ��ƥ���Ҫ��
            mSelectedFeature = results.first().mFeature;
            mIsFeatureSelected = true;

            // �������
            qDebug() << "Selected Feature ID:" << mSelectedFeature.id();

            // ѡ��Ҫ��
            layer->select(mSelectedFeature.id());
        }
        else {
            qDebug() << "No feature selected!";
            mIsFeatureSelected = false;
        }
    }
    else if (event->button() == Qt::RightButton) {
        if (mIsFeatureSelected) {
            // �����Ի������û����뻺��뾶
            bool ok;
            double radius = QInputDialog::getDouble(
                nullptr,
                "Input Buffer Radius",
                "Enter buffer radius:",
                0.0,        // Ĭ��ֵ
                0.0,        // ��Сֵ
                10000.0,    // ���ֵ
                5,          // С����λ��
                &ok
            );

            if (ok) {
                // ִ�л���������
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
    for (QgsMapLayer* layer : layers) { // ȷ����ȷ����
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
        // ����������
        QgsGeometry bufferGeometry = geometry.buffer(radius, 5);  // �뾶�ͻ������ķֶ���

        if (!bufferGeometry.isNull() && bufferGeometry.isGeosValid()) {
            // ����һ���µ�Ҫ�����洢����������
            QgsFeature bufferFeature;
            bufferFeature.setGeometry(bufferGeometry);

            // ����һ���ڴ�ͼ�����洢������
            QgsVectorLayer* bufferLayer = new QgsVectorLayer("Polygon?crs=" + layer->crs().toWkt(), "Buffer Layer", "memory");
            if (bufferLayer->isValid()) {
                QgsVectorDataProvider* provider = bufferLayer->dataProvider();

                // ����Ҫ���б����뻺����Ҫ��
                QList<QgsFeature> features;
                features.append(bufferFeature);

                // ��������Ҫ���б���ӵ�ͼ��
                provider->addFeatures(features);

                // ��������ͼ����ӵ���Ŀ��
                QgsProject::instance()->addMapLayer(bufferLayer);
                qDebug() << "�����������ɹ���";
            }
            else {
                QMessageBox::critical(nullptr, "����", "�޷�����������ͼ�㣡");
            }
        }
        else {
            QMessageBox::critical(nullptr, "����", "������������Ч��");
        }
    }
    else {
        QMessageBox::critical(nullptr, "����", "ѡ�е�Ҫ����Ч��");
    }
}
