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
    // 地图画布
    QgsMapCanvas* m_mapCanvas = nullptr;
    //当前选中的图层
    QgsMapLayer* m_curMapLayer;
    // 图层管理器
    QgsLayerTreeView* m_layerTreeView;
    QgsLayerTreeMapCanvasBridge* m_layerTreeCanvasBridge;
   
    void autoSelectAddedLayer(QList<QgsMapLayer*> layers);
    //矢量，地理处理
    //凸包
   // ConvexHull* m_convexHull;
    //数据处理，ID3
    ID3andC4* m_ID3andC4;

public:
    // 初始化图层管理器
    void initLayerTreeView();
    //图层
    void onCurrentLayerChanged(QgsMapLayer* layer);
public slots:
 void on_actionLayerTreeControl_triggered();
 void on_actionAddVectorLayer_triggered();

 // 移除图层
 void removeLayer();

 //矢量，地理处理
 //凸包
 //void on_actionConvexHull_triggered();

 //数据处理，ID3
 void on_actionID3_triggered();

};
