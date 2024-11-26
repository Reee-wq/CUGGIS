#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CUGGIS.h"
//#include "ConvexHull.h"
#include "ID3andC4.h"
#include <qgsmapcanvas.h>
#include <qgslayertreeview.h>
#include <qgslayertreemodel.h>
#include <qgslayertreemapcanvasbridge.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgsvectorlayer.h>


class CUGGIS : public QMainWindow
{
    Q_OBJECT

public:
    CUGGIS(QWidget *parent = nullptr);
    ~CUGGIS();

private:
    Ui::CUGGISClass ui;
    // ��ͼ����
    QgsMapCanvas* m_mapCanvas = nullptr;
    //��ǰѡ�е�ͼ��
    QgsMapLayer* m_curMapLayer;
    // ͼ�������
    QgsLayerTreeView* m_layerTreeView;
    QgsLayerTreeMapCanvasBridge* m_layerTreeCanvasBridge;
   
    void autoSelectAddedLayer(QList<QgsMapLayer*> layers);
    //ʸ����������
    //͹��
   // ConvexHull* m_convexHull;
    //���ݴ���ID3
    ID3andC4* m_ID3andC4;

public:
    // ��ʼ��ͼ�������
    void initLayerTreeView();
    //ͼ��
    void onCurrentLayerChanged(QgsMapLayer* layer);
public slots:
 void on_actionLayerTreeControl_triggered();
 void on_actionAddVectorLayer_triggered();

 // �Ƴ�ͼ��
 void removeLayer();

 //ʸ����������
 //͹��
 //void on_actionConvexHull_triggered();

 //���ݴ���ID3
 void on_actionID3_triggered();

};
