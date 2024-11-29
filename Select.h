#pragma once
#include <qgsmaptoolidentifyfeature.h>
#include <qgsmapcanvas.h>
#include <qgsfeature.h>
#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <qgsvectorlayerselectionmanager.h> // ������ͷ�ļ�

// �Զ���ʶ�𹤾���
class CustomIdentifyTool : public QgsMapToolIdentifyFeature {
    Q_OBJECT
public:
    explicit CustomIdentifyTool(QgsMapCanvas* canvas);  // ֻ��Ҫ�����ͼ����
    void canvasReleaseEvent(QgsMapMouseEvent* event) override;

private slots:
    void showAttributeEditor();  // ��ʾ���Ա༭��


private:
    QgsVectorLayer* getFirstVectorLayer();  // ��ȡ��һ��ʸ��ͼ��
    void showAttributeEditor(QgsFeature feature);  // ��ʾ���Ա༭��
    QMenu* m_contextMenu;  // �Ҽ��˵�
    QgsVectorLayer* m_currentLayer;  // ��ǰʸ��ͼ��
    QgsFeature m_currentFeature;  // ��ǰѡ�е�Ҫ��
};
