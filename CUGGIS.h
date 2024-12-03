#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include "ui_CUGGIS.h"
#include "ConvexHull.h"
#include "ID3andC4.h"

#include "qgsmaplayer.h"
#include <qgsmapcanvas.h>
#include <qgslayertreeview.h>
#include <QLabel>
#include <qgslayertreemodel.h>
#include <qgslayertreemapcanvasbridge.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgsvectorlayer.h>
#include <qgsrasterlayer.h>
#include"qgsrastercalcdialog.h"
#include <QProgressDialog>


//2用到的
#include<qgsStyle.h>
#include<qgsStyleManagerDialog.h>
#include<QMessageBox>
#include <qgsrenderer.h>
#include <qgssymbol.h>
#include <qgsmapcanvas.h>
#include<qgssinglesymbolrenderer.h>
#include<qgsrasterrenderer.h>
#include"SymbolManager.h"
#include"SymbolModify.h"
#include"Layering.h"

#include <QgsProject.h>
#include <QgsRasterLayer.h>
#include <QgsColorRampShader.h>
#include <QgsStyle.h>
#include <QgsRasterRenderer.h>
#include <QgsPalettedRasterRenderer.h>
#include <QgsRendererRange.h>

//3.1-3.3用到的，地图编辑
#include"AddPoint.h"
#include"deleteTool.h"
#include"moveTool.h"
#include"copyTool.h"
#include"bufferTool.h"
#include "parallelTool.h"
//4用到的
#include "qgscoordinatereferencesystem.h"
#include "Buffer.h"
#include "SHPtoGRID.h"
#include "qgsmaptoolselect.h"
#include "CircleCut.h"
#include "PolygonCut.h"
#include "decisionTree.h"
//#include "total.h"

class CUGGIS : public QMainWindow
{
    Q_OBJECT

public:
    CUGGIS(QWidget *parent = nullptr);
    ~CUGGIS();
    static CUGGIS* instance() {
        static CUGGIS instance;  // 局部静态变量，确保只有一个实例
        return &instance;
    }

private:
    Ui::CUGGISClass ui;
   
 
    void autoSelectAddedLayer(QList<QgsMapLayer*> layers);
    //1.2文件树目录，拖拽打开文件
    QStringList formatList;
    //1.2工具箱树目录
    QTreeView* treeView = nullptr;
    QStandardItemModel* treeModel = nullptr;
    void addLayer(const QString& layerName, QAction* action, const QList<QAction*>& subActions);
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void onTreeViewDoubleClick(const QModelIndex& index);
    //1.5动态投影，鼠标悬停显示坐标
    QLabel* m_statusBarLabel;
    
    //1.6 地图缩放工具
    QgsMapTool* m_zoomInTool;
    QgsMapTool* m_zoomOutTool;

    //4矢量，地理处理，缓冲区和矢量转栅格
    Buffer BufferWindow;// 初始化缓冲区窗口
    SHPtoGRID SHPtoGRIDWindow;// 初始化矢量转栅格窗口
    //4矢量，地理处理，凸包
    ConvexHull* m_convexHull;
    //数据处理，ID3
     //决策树对象指针
    decisionTree* tree;
    treeNode* detree;
    QGraphicsScene* scene;
    QGraphicsView* jctreeView;
    void drawTree();
    void drawTreeRecursive(treeNode* node, int x, int y, int depth);
    void visualizeTree(QGraphicsScene* scene, TreeRoot root, int x, int y, int level, int offset);

public:
    // 图层管理器
    QgsLayerTreeView* m_layerTreeView;
    // 地图画布
    QgsMapCanvas* m_mapCanvas = nullptr;
    //当前选中的图层
    QgsMapLayer* m_curMapLayer;
    QgsLayerTreeMapCanvasBridge* m_layerTreeCanvasBridge;
    // 1.2初始化图层管理器
    void initLayerTreeView();
    void addLayersBelowLast(const QList<QgsMapLayer*>& layerList);
    
    //图层
    void onCurrentLayerChanged(QgsMapLayer* layer);

   //1.5动态投影
    void setLayersCrsToLastLayerCrs();
    //2地图符号与色表
    void on_actionQgsStylelibMng_triggered();  //打开QGIS符号库
    void on_styleManagerDialogFinished(int result);  //样式管理器关闭
    void on_actionSelfStylelibMng_triggered();//打开自定义符号管理器
    void onLayerTreeItemDoubleClicked();  //双击图层修改符号
    SymbolManager* mSymbolManager = nullptr;  //符号管理器窗口
    //3.4Redo/Undo
    AddPoint* m_AddPoint; // 确保指针类型正确
    //4矢量，地理处理
    QgsMapToolSelect* m_pSelectTool;
    CircleCut* m_pCircleCut;
    PolygonCut* m_pPolygonCut;
  //数据处理，ID3andC4
    ID3andC4* m_ID3andC4;
    
public slots:
    //1.1工程,新建工程
    void on_actionNewProject_triggered();
    //1.1工程，打开工程
    void on_actionOpenProject_triggered();
    // 1.1工程，保存工程
    void on_actionSaveProject_triggered();
    void saveProject(const QString& filePath);
    // 1.1工程，另存工程
    void on_actionSaveAsProject_triggered();
    // 1.1工程，退出程序
    void on_actionExit_triggered();
    //1.1图层，图层树目录和添加矢量文件
    void on_actionLayerTreeControl_triggered();
    void on_actionAddVectorLayer_triggered();
    //1.1帮助网址打开
    void on_actionHelp_triggered();
    //1.1图层，添加栅格文件
    void on_actionAddRasterLayer_triggered();
    //1.3图层，图层属性
    void on_actionLayerProperties_triggered();

    // 1.6使用地图放大工具
    void on_actionZoomIn_triggered();
    // 1.6使用地图缩小工具
    void on_actionZoomOut_triggered();
    //1.6使用地图漫游工具
    void on_actionPan_triggered();
     //1.6箭头
    void on_actionExitPan_triggered();
    // 移除图层
    void removeLayer();
    //3-1-3.3地图编辑
    void on_actionAddPoint_triggered();
    void on_actionDelete_triggered();
    void on_actionMove_triggered();
    void on_actionCopy_triggered();
    void on_actionBuffer_2_triggered();
    void on_actionParallel_triggered();

    //3.4查看属性表 DILNUR
    void on_actionOpenAttrTable_triggered();
    //3.4查看字段   DILNUR
    void on_actionOpenFields_triggered();
    // 3.4打开属性表并可编辑
    void openEditableAttributeTable();
    // 3.4触发选中工具
    void on_actionChoose_triggered();
    //3.4触发取消选中工具
    void on_actionFinish_triggered();
    //3.4Undo/Redo
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    //3.5Excel转矢量  DILNUR
    void on_actionExcelShp_triggered();

 //4矢量，地理处理
 void on_actionRasterCalculator_triggered();//栅格计算器
 void on_actionBuffer_triggered();//缓冲区分析
 void on_actionSHPtoGRID_triggered();//矢量转栅格
 void on_actionRectangle_triggered();//矩形拉框
 void on_actionCircle_triggered();//圆形拉框
 void on_actionRandom_triggered();//任意多边形拉框
 //4矢量，地理处理，凸包
 void on_actionConvexHull_triggered();

 //数据处理，ID3
 void on_actionID3_triggered();
 void on_actionC45_triggered();

 protected:
     //1.2从文件树目录拖拽，打开文件
     void dragEnterEvent(QDragEnterEvent* event);
     void dropEvent(QDropEvent* event);
     void projectOpen(const QString& filepath);
     void addLayer(const QString& filepath);

 private slots:
     void onAction1Triggered();
     void onAction2Triggered();
     void onAction3Triggered();
     void onAction4Triggered();
     void onAction5Triggered();
       


};
