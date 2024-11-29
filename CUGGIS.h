#pragma once
#pragma execution_character_set("utf-8")

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
//4�õ���
#include "qgscoordinatereferencesystem.h"
#include "Buffer.h"
#include "SHPtoGRID.h"
#include "qgsmaptoolselect.h"
#include "CircleCut.h"
#include "PolygonCut.h"


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
    
    //1.6 ��ͼ���Ź���
    QgsMapTool* m_zoomInTool;
    QgsMapTool* m_zoomOutTool;

    //4ʸ������������������ʸ��תդ��
    Buffer BufferWindow;// ��ʼ������������
    SHPtoGRID SHPtoGRIDWindow;// ��ʼ��ʸ��תդ�񴰿�
    //4ʸ����������͹��
   // ConvexHull* m_convexHull;
    //���ݴ���ID3
    ID3andC4* m_ID3andC4;

public:
    // ͼ�������
    QgsLayerTreeView* m_layerTreeView;
    QgsLayerTreeMapCanvasBridge* m_layerTreeCanvasBridge;
    // 1.2��ʼ��ͼ�������
    void initLayerTreeView();
    void addLayersBelowLast(const QList<QgsMapLayer*>& layerList);
    //ͼ��
    void onCurrentLayerChanged(QgsMapLayer* layer);

   //1.5��̬ͶӰ
    void setLayersCrsToLastLayerCrs();
   /* void showCoordinates(const QgsPointXY& coordinate);*/
    //2��ͼ������ɫ��
    void on_actionQgsStylelibMng_triggered();  //��QGIS���ſ�
    void on_styleManagerDialogFinished(int result);  //��ʽ�������ر�
    void on_actionSelfStylelibMng_triggered();//���Զ�����Ź�����
    void onLayerTreeItemDoubleClicked();  //˫��ͼ���޸ķ���
    SymbolManager* mSymbolManager = nullptr;  //���Ź���������

    //4ʸ����������
    QgsMapToolSelect* m_pSelectTool;
    CircleCut* m_pCircleCut;
    PolygonCut* m_pPolygonCut;
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

 //4ʸ����������
 void on_actionRasterCalculator_triggered();//դ�������
 void on_actionBuffer_triggered();//����������
 void on_actionSHPtoGRID_triggered();//ʸ��תդ��
 void on_actionRectangle_triggered();//��������
 void on_actionCircle_triggered();//Բ������
 void on_actionRandom_triggered();//������������
 //͹��
 //void on_actionConvexHull_triggered();

 //���ݴ���ID3
 void on_actionID3_triggered();

//protected:
//    void resizeEvent(QResizeEvent* event);

};
