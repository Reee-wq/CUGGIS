#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_total.h"
//#include"chessBoard.h"
#include"convexHull.h"
//#include"Navigate.h"
#include"decisionTree.h"
//#include"IDW.h"
//#include"tin.h"
//#include"tools.h"
//#include"myQgraphicsScene.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>

class total : public QMainWindow
{
    Q_OBJECT

public:
    total(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent* event);
    ~total();
private slots:
    //��һ��
    void openChessFile(); //���ļ�
    void calMaxArea(); //���������������
    //�ڶ���
    void openHullFile(); //���ļ�
    void calConvexHull(); //����͹��
    //������
    void ID3CreateTree(); //���ļ�����ID3�㷨������
    void C45CreateTree();//���ļ�����C45�㷨������
    void predict(); //Ԥ��Լ����
    //������
    void openSchoolFile(); //���ļ�
    void displayPlaceInfo(); //��ʾ������Ϣ
    void calShortestPath(); //�������·��
    void displayShortestPath();//��ʾ���·��
    //������
    void openIDW_File();//��͹�������ļ�
    void openContour();//�򿪵ȸ���json�ļ�
    void onNearNumTextChanged(); //���������ٽ������
    //������
    void CreateTIN(); //����TIN
    void findPush(); //��ѯ

private:
    Ui::totalClass ui;
    //����
    QPixmap* pixmap;
    QGraphicsScene* scene;
    //MyGraphicsScene* navigScene;
    //�����ڵ����·��
    QGraphicsPathItem* newPath = nullptr;
    QGraphicsEllipseItem* newPoint = nullptr;
    //ѡȡ�����յ�
    int startOrEnd = 0;
    int start, end;
    //Tin��Χ
    double x_extent, y_extent;

    //���̶���ָ��
    chessBoard *chess;
    //͹������ָ��
    convexHull* hull;
    //����������ָ��
    decisionTree* tree;
    treeNode* detree;
    //��������ָ��
    Navigate* navig;
    //IDW����ָ��
    IDW *idw;
    //TIN����ָ��
    Tin *tin;
    tools*tool_form;

    //�ڶ�����Ƶ�
    void drawCoords();
    //��������ƾ�����
    void drawTree();
    void drawTreeRecursive(treeNode* node, int x, int y, int depth);
    //�������ͼ�������·������
    void drawPath(int start,int end); //·������
    void drawStart(int start); //�������Լ����·������
    void dispalyInfo(int place);//��ʾ������Ϣ
    //������TIN����
    QPolygonF getScreenTriangle(Triangle* t);
    QPointF getScreenPoint(Pixel);
    
};
