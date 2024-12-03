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


//2�õ���
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

//3.1-3.3�õ��ģ���ͼ�༭
#include"AddPoint.h"
#include"deleteTool.h"
#include"moveTool.h"
#include"copyTool.h"
#include"bufferTool.h"
#include "parallelTool.h"
//4�õ���
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
        static CUGGIS instance;  // �ֲ���̬������ȷ��ֻ��һ��ʵ��
        return &instance;
    }

private:
    Ui::CUGGISClass ui;
   
 
    void autoSelectAddedLayer(QList<QgsMapLayer*> layers);
    //1.2�ļ���Ŀ¼����ק���ļ�
    QStringList formatList;
    //1.2��������Ŀ¼
    QTreeView* treeView = nullptr;
    QStandardItemModel* treeModel = nullptr;
    void addLayer(const QString& layerName, QAction* action, const QList<QAction*>& subActions);
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void onTreeViewDoubleClick(const QModelIndex& index);
    //1.5��̬ͶӰ�������ͣ��ʾ����
    QLabel* m_statusBarLabel;
    
    //1.6 ��ͼ���Ź���
    QgsMapTool* m_zoomInTool;
    QgsMapTool* m_zoomOutTool;

    //4ʸ������������������ʸ��תդ��
    Buffer BufferWindow;// ��ʼ������������
    SHPtoGRID SHPtoGRIDWindow;// ��ʼ��ʸ��תդ�񴰿�
    //4ʸ����������͹��
    ConvexHull* m_convexHull;
    //���ݴ���ID3
     //����������ָ��
    decisionTree* tree;
    treeNode* detree;
    QGraphicsScene* scene;
    QGraphicsView* jctreeView;
    void drawTree();
    void drawTreeRecursive(treeNode* node, int x, int y, int depth);
    void visualizeTree(QGraphicsScene* scene, TreeRoot root, int x, int y, int level, int offset);

public:
    // ͼ�������
    QgsLayerTreeView* m_layerTreeView;
    // ��ͼ����
    QgsMapCanvas* m_mapCanvas = nullptr;
    //��ǰѡ�е�ͼ��
    QgsMapLayer* m_curMapLayer;
    QgsLayerTreeMapCanvasBridge* m_layerTreeCanvasBridge;
    // 1.2��ʼ��ͼ�������
    void initLayerTreeView();
    void addLayersBelowLast(const QList<QgsMapLayer*>& layerList);
    
    //ͼ��
    void onCurrentLayerChanged(QgsMapLayer* layer);

   //1.5��̬ͶӰ
    void setLayersCrsToLastLayerCrs();
    //2��ͼ������ɫ��
    void on_actionQgsStylelibMng_triggered();  //��QGIS���ſ�
    void on_styleManagerDialogFinished(int result);  //��ʽ�������ر�
    void on_actionSelfStylelibMng_triggered();//���Զ�����Ź�����
    void onLayerTreeItemDoubleClicked();  //˫��ͼ���޸ķ���
    SymbolManager* mSymbolManager = nullptr;  //���Ź���������
    //3.4Redo/Undo
    AddPoint* m_AddPoint; // ȷ��ָ��������ȷ
    //4ʸ����������
    QgsMapToolSelect* m_pSelectTool;
    CircleCut* m_pCircleCut;
    PolygonCut* m_pPolygonCut;
  //���ݴ���ID3andC4
    ID3andC4* m_ID3andC4;
    
public slots:
    //1.1����,�½�����
    void on_actionNewProject_triggered();
    //1.1���̣��򿪹���
    void on_actionOpenProject_triggered();
    // 1.1���̣����湤��
    void on_actionSaveProject_triggered();
    void saveProject(const QString& filePath);
    // 1.1���̣���湤��
    void on_actionSaveAsProject_triggered();
    // 1.1���̣��˳�����
    void on_actionExit_triggered();
    //1.1ͼ�㣬ͼ����Ŀ¼�����ʸ���ļ�
    void on_actionLayerTreeControl_triggered();
    void on_actionAddVectorLayer_triggered();
    //1.1������ַ��
    void on_actionHelp_triggered();
    //1.1ͼ�㣬���դ���ļ�
    void on_actionAddRasterLayer_triggered();
    //1.3ͼ�㣬ͼ������
    void on_actionLayerProperties_triggered();

    // 1.6ʹ�õ�ͼ�Ŵ󹤾�
    void on_actionZoomIn_triggered();
    // 1.6ʹ�õ�ͼ��С����
    void on_actionZoomOut_triggered();
    //1.6ʹ�õ�ͼ���ι���
    void on_actionPan_triggered();
     //1.6��ͷ
    void on_actionExitPan_triggered();
    // �Ƴ�ͼ��
    void removeLayer();
    //3-1-3.3��ͼ�༭
    void on_actionAddPoint_triggered();
    void on_actionDelete_triggered();
    void on_actionMove_triggered();
    void on_actionCopy_triggered();
    void on_actionBuffer_2_triggered();
    void on_actionParallel_triggered();

    //3.4�鿴���Ա� DILNUR
    void on_actionOpenAttrTable_triggered();
    //3.4�鿴�ֶ�   DILNUR
    void on_actionOpenFields_triggered();
    // 3.4�����Ա��ɱ༭
    void openEditableAttributeTable();
    // 3.4����ѡ�й���
    void on_actionChoose_triggered();
    //3.4����ȡ��ѡ�й���
    void on_actionFinish_triggered();
    //3.4Undo/Redo
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    //3.5Excelתʸ��  DILNUR
    void on_actionExcelShp_triggered();

 //4ʸ����������
 void on_actionRasterCalculator_triggered();//դ�������
 void on_actionBuffer_triggered();//����������
 void on_actionSHPtoGRID_triggered();//ʸ��תդ��
 void on_actionRectangle_triggered();//��������
 void on_actionCircle_triggered();//Բ������
 void on_actionRandom_triggered();//������������
 //4ʸ����������͹��
 void on_actionConvexHull_triggered();

 //���ݴ���ID3
 void on_actionID3_triggered();
 void on_actionC45_triggered();

 protected:
     //1.2���ļ���Ŀ¼��ק�����ļ�
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
