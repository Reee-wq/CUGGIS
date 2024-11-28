#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CUGGIS.h"
//#include "ConvexHull.h"
#include "ID3andC4.h"
#include "qgsmaplayer.h"
#include <qgsmapcanvas.h>
#include <qgslayertreeview.h>
#include <QLabel>
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
   
   
    void autoSelectAddedLayer(QList<QgsMapLayer*> layers);
    //1.5��̬ͶӰ�������ͣ��ʾ����
    QLabel* m_statusBarLabel;
    //ʸ����������
    //͹��
   // ConvexHull* m_convexHull;
    //���ݴ���ID3
    ID3andC4* m_ID3andC4;

public:
    // ͼ�������
    QgsLayerTreeView* m_layerTreeView;
    QgsLayerTreeMapCanvasBridge* m_layerTreeCanvasBridge;
    // 1.2��ʼ��ͼ�������
    void initLayerTreeView();
    //ͼ��
    void onCurrentLayerChanged(QgsMapLayer* layer);

   //1.5��̬ͶӰ
    void setLayersCrsToFirstLayerCrs();
   /* void showCoordinates(const QgsPointXY& coordinate);*/
public slots:
    //1.1ͼ�㣬ͼ����Ŀ¼�����ʸ���ļ�
 void on_actionLayerTreeControl_triggered();
 void on_actionAddVectorLayer_triggered();
 //1.1ͼ�㣬���դ���ļ�
 void on_actionAddRasterLayer_triggered();
 //1.3ͼ�㣬ͼ������
 void on_actionLayerProperties_triggered();


 // �Ƴ�ͼ��
 void removeLayer();

 //ʸ����������
 //͹��
 //void on_actionConvexHull_triggered();

 //���ݴ���ID3
 void on_actionID3_triggered();

};
