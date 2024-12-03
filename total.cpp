#include "total.h"
#include<qpainter.h>
#include<Qmessagebox.h>
#include<qdebug.h>
#include<qmouseevent>
#include<qmessagebox.h>
#include<QGraphicsSceneMouseEvent>
#pragma execution_character_set("utf-8")

total::total(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.openChessFile, &QPushButton::clicked, this, &total::openChessFile);
    connect(ui.calMaxArea, &QPushButton::clicked, this, &total::calMaxArea);
    connect(ui.openHullFile, &QPushButton::clicked, this, &total::openHullFile);
    connect(ui.calConvexHull, &QPushButton::clicked, this, &total::calConvexHull);
    connect(ui.ID3PushButton, &QPushButton::clicked, this, &total::ID3CreateTree);
    connect(ui.C45PushButton, &QPushButton::clicked, this, &total::C45CreateTree);
    connect(ui.predictPushButton, &QPushButton::clicked, this, &total::predict);
    connect(ui.openSchoolFile, &QPushButton::clicked, this, &total::openSchoolFile);
    connect(ui.placeInfoBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &total::displayPlaceInfo);
    connect(ui.startBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &total::calShortestPath);
    connect(ui.endBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &total::displayShortestPath);
    connect(ui.openContour, &QPushButton::clicked, this, &total::openContour);
    connect(ui.openIDW_File, &QPushButton::clicked, this, &total::openIDW_File);
    connect(ui.nearLineEdit, &QLineEdit::editingFinished, this, &total::onNearNumTextChanged);
    connect(ui.CreateTINpushButton, &QPushButton::clicked, this, &total::CreateTIN);
    connect(ui.findPushButton, &QPushButton::clicked, this, &total::findPush);
    ui.IndexLineEdit->setText("输入一个点序号");
}

total::~total()
{}

void total::mousePressEvent(QMouseEvent* event) {
    //凸包点编辑
    if (ui.startEditHull->isChecked()) {
        //获取屏幕坐标
        QPoint pos = QCursor::pos();
        //获取视图坐标
        QPoint view = ui.convexHullView->mapFromGlobal(pos);
        //获取场景坐标
        QPointF scenePos = ui.convexHullView->mapToScene(view);
        double newX = (scenePos.x()) / 15;
        double newY = (scenePos.y()) / 15;
        int newSize = this->hull->coordsData.size() + 1;
        this->hull->coordsData.push_back(hull_Point{ newX,newY,newSize });
        //绘制坐标点
        this->drawCoords();
    }

    //导航
    if (ui.startNavigate->isChecked()) {
        QPoint pos = QCursor::pos();
        QPoint view = ui.navigateView->mapFromGlobal(pos);
        QPointF scenePos = ui.navigateView->mapToScene(view);
        //获取起点索引
        QGraphicsItem* item = navigScene->itemAt(scenePos, ui.navigateView->transform());
        int index = navigScene->getItemIndex(item);
        //鼠标右击显示信息
        if (event->button() == Qt::RightButton) {
            this->dispalyInfo(index);
        }
        //开始导航
        else {
            //起点选取并计算其到各点的最短路径
            if (index > -1 && startOrEnd == 0) {
                this->start = index;
                this->drawStart(this->start);
                startOrEnd = 1;
            }
            //终点选取绘制最短路径
            else if (index > -1 && startOrEnd == 1) {
                this->end = index;
                this->drawPath(this->start, this->end);
                startOrEnd = 0;
            }
        }
    }
    
    //反距离加权结果显示
    if (ui.startInsert->isChecked()) {
        //判断是否输入所需临近点个数
        if (ui.nearLineEdit->text().isEmpty()) {
            QMessageBox::warning(nullptr, "warn", "nearNum is null");
            return;
        }
        //获取屏幕坐标
        QPoint pos = QCursor::pos();
        //获取视图坐标
        QPoint view = ui.IDW_View->mapFromGlobal(pos);
        //获取场景坐标
        QPointF scenePos = ui.IDW_View->mapToScene(view);
        //坐标转换，将鼠标点击位置的坐标转换为与样本数据一致的坐标系内
        double newX = scenePos.x() / this->idw->scale;
        double newY= scenePos.y() / this->idw->scale;
        //添加未知点
        QGraphicsEllipseItem* ellipseItem = scene->addEllipse(scenePos.x() - 2, scenePos.y() - 2, 4, 4);
        ellipseItem->setBrush(Qt::green);
        //计算高程值
        double elev=this->idw->calElev(newX,newY);
        // 将坐标值和高程值转换为字符串
        QString pointInfo = "X: " + QString::number(newX,'f',10) + ", Y: " + QString::number(newY, 'f', 10);
        QString elevInfo = "Elevation: " + QString::number(elev, 'f', 1);
        // 创建一个弹出框并显示当前点的信息
        QMessageBox::information(nullptr, "Data Information", pointInfo + "\n" + elevInfo);
    }
}