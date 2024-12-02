#include "deleteTool.h"
#include <qgsproject.h>
#include <qgsgeometry.h>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include<qgsmapmouseevent.h>

// ���캯��
CustomDeleteTool::CustomDeleteTool(QgsMapCanvas* canvas)
    : QgsMapToolIdentifyFeature(canvas), mCtrlPressed(false) {}

// ����ͷ��¼�����
void CustomDeleteTool::canvasReleaseEvent(QgsMapMouseEvent* event) {
    // ��ȡ��һ��ʸ��ͼ��
    QgsVectorLayer* layer = getFirstVectorLayer();
    if (!layer) {
        qDebug() << "No vector layers found!";
        return;
    }

    // ��ȡ���������Ļ����
    int x = event->pos().x();
    int y = event->pos().y();

    // ʹ�� identify �������пռ��ѯ
    QList<QgsMapToolIdentify::IdentifyResult> results = identify(
        x, y,                                // ��Ļ����
        QList<QgsMapLayer*>{layer},          // ָ����ѯ��ͼ��
        QgsMapToolIdentify::TopDownStopAtFirst // ��ѯģʽ���ҵ���һ��ƥ���Ҫ��
    );

    if (!results.isEmpty()) {
        // ��ȡ��һ��ƥ���Ҫ��
        QgsFeature feature = results.first().mFeature;

        if (mCtrlPressed) {
            // ��� Ctrl �����£�����ӵ�ѡ�м�����
            mSelectedFeatureIds.insert(feature.id());
            qDebug() << "Added feature ID" << feature.id() << "to selection.";
        }
        else {
            // ���û�а��� Ctrl ������ֻ����ǰ�����Ҫ��
            mSelectedFeatureIds.clear();
            mSelectedFeatureIds.insert(feature.id());

            // ִ��ɾ��
            deleteFeatures(layer, QList<QgsFeatureId>{feature.id()});
        }
    }
    else {
        qDebug() << "No feature selected!";
    }
}

// ��ȡ��һ��ʸ��ͼ��
QgsVectorLayer* CustomDeleteTool::getFirstVectorLayer() {
    auto layers = QgsProject::instance()->mapLayers().values();
    for (auto layer : layers) {
        QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(layer);
        if (vectorLayer) {
            return vectorLayer;  // �����ҵ��ĵ�һ��ʸ��ͼ��
        }
    }
    return nullptr;  // ���û��ʸ��ͼ�㣬���ؿ�ָ��
}

// ɾ��ѡ�е�Ҫ��
void CustomDeleteTool::deleteFeatures(QgsVectorLayer* layer, const QList<QgsFeatureId>& featureIds) {
    if (!layer->isEditable()) {
        layer->startEditing();  // �����༭ģʽ
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

    // �ύ�༭
    if (success && layer->commitChanges()) {
        qDebug() << "Changes committed successfully.";
        emit featuresDeleted(QString("Successfully deleted %1 feature(s).").arg(featureIds.size()));
    }
    else {
        qDebug() << "Failed to commit changes to the layer.";
        layer->rollBack();  // ����ύʧ�ܣ��ع��༭
    }

    // ���ѡ�м���
    mSelectedFeatureIds.clear();
}

// ���̰����¼�����
void CustomDeleteTool::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = true;
        qDebug() << "Ctrl key pressed.";
    }
}

// �����ͷ��¼�����
void CustomDeleteTool::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        mCtrlPressed = false;
        qDebug() << "Ctrl key released.";

        // ����ɾ����ѡ�е�Ҫ��
        QgsVectorLayer* layer = getFirstVectorLayer();
        if (layer && !mSelectedFeatureIds.isEmpty()) {
            deleteFeatures(layer, mSelectedFeatureIds.values());
        }
    }
}
