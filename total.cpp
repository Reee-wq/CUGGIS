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
    ui.IndexLineEdit->setText("����һ�������");
}

total::~total()
{}

void total::mousePressEvent(QMouseEvent* event) {
    //͹����༭
    if (ui.startEditHull->isChecked()) {
        //��ȡ��Ļ����
        QPoint pos = QCursor::pos();
        //��ȡ��ͼ����
        QPoint view = ui.convexHullView->mapFromGlobal(pos);
        //��ȡ��������
        QPointF scenePos = ui.convexHullView->mapToScene(view);
        double newX = (scenePos.x()) / 15;
        double newY = (scenePos.y()) / 15;
        int newSize = this->hull->coordsData.size() + 1;
        this->hull->coordsData.push_back(hull_Point{ newX,newY,newSize });
        //���������
        this->drawCoords();
    }

    //����
    if (ui.startNavigate->isChecked()) {
        QPoint pos = QCursor::pos();
        QPoint view = ui.navigateView->mapFromGlobal(pos);
        QPointF scenePos = ui.navigateView->mapToScene(view);
        //��ȡ�������
        QGraphicsItem* item = navigScene->itemAt(scenePos, ui.navigateView->transform());
        int index = navigScene->getItemIndex(item);
        //����һ���ʾ��Ϣ
        if (event->button() == Qt::RightButton) {
            this->dispalyInfo(index);
        }
        //��ʼ����
        else {
            //���ѡȡ�������䵽��������·��
            if (index > -1 && startOrEnd == 0) {
                this->start = index;
                this->drawStart(this->start);
                startOrEnd = 1;
            }
            //�յ�ѡȡ�������·��
            else if (index > -1 && startOrEnd == 1) {
                this->end = index;
                this->drawPath(this->start, this->end);
                startOrEnd = 0;
            }
        }
    }
    
    //�������Ȩ�����ʾ
    if (ui.startInsert->isChecked()) {
        //�ж��Ƿ����������ٽ������
        if (ui.nearLineEdit->text().isEmpty()) {
            QMessageBox::warning(nullptr, "warn", "nearNum is null");
            return;
        }
        //��ȡ��Ļ����
        QPoint pos = QCursor::pos();
        //��ȡ��ͼ����
        QPoint view = ui.IDW_View->mapFromGlobal(pos);
        //��ȡ��������
        QPointF scenePos = ui.IDW_View->mapToScene(view);
        //����ת�����������λ�õ�����ת��Ϊ����������һ�µ�����ϵ��
        double newX = scenePos.x() / this->idw->scale;
        double newY= scenePos.y() / this->idw->scale;
        //���δ֪��
        QGraphicsEllipseItem* ellipseItem = scene->addEllipse(scenePos.x() - 2, scenePos.y() - 2, 4, 4);
        ellipseItem->setBrush(Qt::green);
        //����߳�ֵ
        double elev=this->idw->calElev(newX,newY);
        // ������ֵ�͸߳�ֵת��Ϊ�ַ���
        QString pointInfo = "X: " + QString::number(newX,'f',10) + ", Y: " + QString::number(newY, 'f', 10);
        QString elevInfo = "Elevation: " + QString::number(elev, 'f', 1);
        // ����һ����������ʾ��ǰ�����Ϣ
        QMessageBox::information(nullptr, "Data Information", pointInfo + "\n" + elevInfo);
    }
}