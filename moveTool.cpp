#include "moveTool.h"
#include <qgsproject.h>
#include <qgsgeometry.h>
#include <QDebug>
#include <QMessageBox>
#include<qgsmapmouseevent.h>

// ���캯��
CustomMoveTool::CustomMoveTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), mDragging(false) {}

// ��ȡ��һ��ʸ��ͼ��
QgsVectorLayer* CustomMoveTool::getFirstVectorLayer() {
    auto layers = QgsProject::instance()->mapLayers().values();
    for (auto layer : layers) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer;  // �����ҵ��ĵ�һ��ʸ��ͼ��
        }
    }
    return nullptr;  // ���û��ʸ��ͼ�㣬���ؿ�ָ��
}

void CustomMoveTool::canvasPressEvent(QgsMapMouseEvent* event) {
    // ��ȡ��һ��ʸ��ͼ��
    QgsVectorLayer* layer = getFirstVectorLayer();
    if (!layer) {
        qDebug() << "No vector layers found!";
        return;
    }

    // ��ȡ���������Ļ����
    int x = event->pos().x();
    int y = event->pos().y();

    // ����Ƿ���Ctrl��
    mCtrlPressed = event->modifiers() & Qt::ControlModifier;

    if (mCtrlPressed) {
        // ����Ctrl��������ѡ��ģʽ
        QList<QgsMapToolIdentify::IdentifyResult> results = identify(
            x, y,
            QList<QgsMapLayer*>{layer},
            QgsMapToolIdentify::TopDownStopAtFirst
        );

        if (!results.isEmpty()) {
            QgsFeature feature = results.first().mFeature;
            if (!mSelectedFeatures.contains(feature)) {
                mSelectedFeatures.append(feature);  // ����ѡ���б�
                qDebug() << "Feature added to selection with ID:" << feature.id();
            }
        }
    }
    else if (!mSelectedFeatures.isEmpty()) {
        // �ɿ�Ctrl�����ٴε����ʼ��¼ƫ���յ�
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

    // ��ȡ����ͷ�ʱ��λ��
    QgsPointXY releaseMousePoint = toMapCoordinates(event->pos());

    // �����һ��Ҫ�ص�ƫ����
    QgsFeature firstFeature = mSelectedFeatures.first();
    QgsGeometry firstGeometry = firstFeature.geometry();
    double deltaX = releaseMousePoint.x() - firstGeometry.centroid().asPoint().x();
    double deltaY = releaseMousePoint.y() - firstGeometry.centroid().asPoint().y();

    // ȷ��ͼ�㴦�ڱ༭״̬
    if (!layer->isEditable()) {
        layer->startEditing();
    }

    // ��������ѡ��Ҫ�صļ�����
    for (QgsFeature& feature : mSelectedFeatures) {
        QgsGeometry geometry = feature.geometry();
        geometry.translate(deltaX, deltaY);
        layer->changeGeometry(feature.id(), geometry);
    }

    // �ύ�޸�
    if (!layer->commitChanges()) {
        qDebug() << "Failed to save changes to the layer.";
        layer->rollBack();
    }
    else {
        qDebug() << "Selected features moved successfully.";
        emit featuresMoved("Selected features moved successfully.");
    }

    // ���ѡ�е�Ҫ���б�������ק״̬
    mSelectedFeatures.clear();
    mDragging = false;
}
