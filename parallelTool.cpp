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
    setCursor(Qt::CrossCursor);  // ����ʮ�ֹ��
    qDebug() << "CustomParallelTool activated.";
}

CustomParallelTool::~CustomParallelTool()
{
    // ��������
}

QgsVectorLayer* CustomParallelTool::getFirstVectorLayer() {
    auto layers = QgsProject::instance()->mapLayers().values();
    for (auto layer : layers) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer;  // �����ҵ��ĵ�һ��ʸ��ͼ��
        }
    }
    return nullptr;  // ���û��ʸ��ͼ�㣬���ؿ�ָ��
}

void CustomParallelTool::canvasPressEvent(QgsMapMouseEvent* event)
{
    // ��ȡ��һ��ʸ��ͼ��
    QgsVectorLayer* layer = getFirstVectorLayer();
    if (!layer) {
        qDebug() << "No vector layers found!";
        return;
    }

    // �������������ѡ��Ҫ��
    if (event->button() == Qt::LeftButton) {
        QList<QgsMapToolIdentify::IdentifyResult> results = identify(
            event->pos().x(), event->pos().y(),               // ��Ļ����
            QList<QgsMapLayer*>{layer},           // ָ����ѯ��ͼ��
            QgsMapToolIdentify::TopDownStopAtFirst // ��ѯģʽ���ҵ���һ��ƥ���Ҫ��
        );

        if (!results.isEmpty()) {
            mSelectedFeature = results.first().mFeature;
            qDebug() << "Selected feature ID:" << mSelectedFeature.id();

            // ѡ��Ҫ��
            layer->select(mSelectedFeature.id());

            // ���Ҫ�����ԣ���ѡ��
            auto attributes = mSelectedFeature.attributes();
            qDebug() << "Attributes:" << attributes;
        }
        else {
            qDebug() << "No feature found at clicked position!";
            mSelectedFeature = QgsFeature(); // ������ѡҪ��
        }
    }

    // ������Ҽ���������� Ctrl �����£�ִ�и����߼�
    if (event->button() == Qt::RightButton && mCtrlPressed) {
        mCtrlPressed = false; // ���ñ�ǣ������ظ�����

        // ȷ��ѡ������ЧҪ��
        if (mSelectedFeature.geometry().isNull()) {
            qDebug() << "No feature selected for copying!";
            return;
        }

        // �����Ի������û�����ƽ���ߵļ��
        bool ok;
        double interval = QInputDialog::getDouble(
            nullptr,
            "Input Parallel Line Spacing",
            "Enter parallel line spacing (positive for right, negative for left):",
            0.1,        // Ĭ��ֵ
            -10000.0,   // ��Сֵ
            10000.0,    // ���ֵ
            2,          // С����λ��
            &ok
        );

        if (!ok) {
            qDebug() << "User canceled the input!";
            return;
        }

        // ȷ��ͼ�㴦�ڱ༭ģʽ
        if (!layer->isEditable()) {
            if (!layer->startEditing()) {
                qDebug() << "Failed to start editing mode for the layer.";
                return;
            }
        }

        // ��ȡ�Ҽ����λ�ã���Ϊ���ƵĻ�׼λ��
        QgsPointXY basePos = mCanvas->getCoordinateTransform()->toMapCoordinates(event->pos());

        // ��ȡҪ�صļ�����
        QgsGeometry geometry = mSelectedFeature.geometry();
        QgsGeometry originalGeometry = QgsGeometry::fromWkt(geometry.asWkt()); // ���������

        // ����ԭʼҪ�ص����ĵ�
        QgsPointXY centroid = originalGeometry.centroid().asPoint();

        // �����Ҳ��ƽ���߼�����
        QgsGeometry rightGeometry = QgsGeometry::fromWkt(originalGeometry.asWkt());
        rightGeometry.translate(interval, 0); // ����ƽ��
        QgsFeature rightFeature = mSelectedFeature;
        rightFeature.setGeometry(rightGeometry);

        // ��������ƽ���߼�����
        QgsGeometry leftGeometry = QgsGeometry::fromWkt(originalGeometry.asWkt());
        leftGeometry.translate(-interval, 0); // ����ƽ��
        QgsFeature leftFeature = mSelectedFeature;
        leftFeature.setGeometry(leftGeometry);

        // ��ƽ������ӵ�ͼ��
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

        // �ύ���Ĳ�ˢ�µ�ͼ����
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
    // ���� Ctrl����ǽ��븴��ģʽ
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = true;
        qDebug() << "Ctrl pressed. Ready for copying.";
    }
}

void CustomParallelTool::keyReleaseEvent(QKeyEvent* event) {
    // �ɿ� Ctrl ʱ���ñ��
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = false;
        qDebug() << "Ctrl released.";
    }
}
