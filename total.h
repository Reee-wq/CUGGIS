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
    //第一题
    void openChessFile(); //打卡文件
    void calMaxArea(); //计算最大连续区域
    //第二题
    void openHullFile(); //打开文件
    void calConvexHull(); //计算凸包
    //第三题
    void ID3CreateTree(); //打开文件并用ID3算法构建树
    void C45CreateTree();//打开文件并用C45算法构建树
    void predict(); //预测约会结果
    //第四题
    void openSchoolFile(); //打开文件
    void displayPlaceInfo(); //显示场所信息
    void calShortestPath(); //计算最短路径
    void displayShortestPath();//显示最短路径
    //第五题
    void openIDW_File();//打开凸包样本文件
    void openContour();//打开等高线json文件
    void onNearNumTextChanged(); //输入所需临近点个数
    //第六题
    void CreateTIN(); //构建TIN
    void findPush(); //查询

private:
    Ui::totalClass ui;
    //绘制
    QPixmap* pixmap;
    QGraphicsScene* scene;
    //MyGraphicsScene* navigScene;
    //导航内的最短路径
    QGraphicsPathItem* newPath = nullptr;
    QGraphicsEllipseItem* newPoint = nullptr;
    //选取起点和终点
    int startOrEnd = 0;
    int start, end;
    //Tin范围
    double x_extent, y_extent;

    //棋盘对象指针
    chessBoard *chess;
    //凸包对象指针
    convexHull* hull;
    //决策树对象指针
    decisionTree* tree;
    treeNode* detree;
    //导航对象指针
    Navigate* navig;
    //IDW对象指针
    IDW *idw;
    //TIN对象指针
    Tin *tin;
    tools*tool_form;

    //第二题绘制点
    void drawCoords();
    //第三题绘制决策树
    void drawTree();
    void drawTreeRecursive(treeNode* node, int x, int y, int depth);
    //第四题地图导航最短路径绘制
    void drawPath(int start,int end); //路径绘制
    void drawStart(int start); //起点绘制以及最短路径计算
    void dispalyInfo(int place);//显示场所信息
    //第六题TIN构建
    QPolygonF getScreenTriangle(Triangle* t);
    QPointF getScreenPoint(Pixel);
    
};
