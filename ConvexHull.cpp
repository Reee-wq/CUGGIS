#include "ConvexHull.h"
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include "qevent.h"

ConvexHull::ConvexHull(QWidget *parent)
	: QWidget(parent)
{
  
	ui.setupUi(this);
    lineEditPoiCount = ui.lineEditPoiCount; // ��������lineEditPoiCount��ui�ļ��еĶ�������lineEditPoiCount
    lineEditPoiCoords = ui.lineEditPoiCoords; // ��������lineEditPoiCoords��ui�ļ��еĶ�������lineEditPoiCoords
    // ����lineEditΪֻ��
    lineEditPoiCount->setReadOnly(true);
    lineEditPoiCoords->setReadOnly(true);
    //// ����sizePolicy
    //lineEditPoiCount->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    //lineEditPoiCoords->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    //// ��ȡQPushButton�ؼ�
    //QPushButton* startButton = ui.Start;
    //QPushButton* exportButton = ui.Export;
    //QPushButton* clearButton = ui.Clear;
    //startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    //exportButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    //clearButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    adjustSize();  // ʹ�����ڴ�С����Ӧ���ݴ�С
}

ConvexHull::~ConvexHull()
{}

int ConvexHull::crossProduct(const QPoint & O, const QPoint & A, const QPoint & B)
{
    return (A.x() - O.x()) * (B.y() - O.y()) - (A.y() - O.y()) * (B.x() - O.x());
  
}

int ConvexHull::distanceSquared(const QPoint& A, const QPoint& B)
{
    return (A.x() - B.x()) * (A.x() - B.x()) + (A.y() - B.y()) * (A.y() - B.y());
}

bool ConvexHull::compare(const QPoint& a, const QPoint& b, const QPoint& start)
{
    int cp = crossProduct(start, a, b);
    if (cp == 0) {
        return distanceSquared(start, a) < distanceSquared(start, b);
    }
    return cp > 0;
}

void ConvexHull::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // ���ÿ����Ч��

    // ���õ����ɫ�ʹ�С
    painter.setPen(Qt::black);  // ������ɫ
    painter.setBrush(Qt::green); // �����ɫ

    // �������е�
    for (const PointWithIndex& point : pointsWithIndex) {
        painter.drawEllipse(point.point, 3, 3);  // ���ư뾶Ϊ3��Բ�ε�
    }
    // ����Ѿ�������͹��������͹��
    if (!convexHull.isEmpty()) {
        painter.setPen(Qt::blue);  // ͹������ɫ
        for (int i = 0; i < convexHull.size(); ++i) {
            int j = (i + 1) % convexHull.size();  // ѭ���������һ����͵�һ����
            painter.drawLine(convexHull[i].point, convexHull[j].point);
        }
    }
    // �������е�ͱ��
    painter.setPen(Qt::red);
    for (const PointWithIndex& point : pointsWithIndex) {
        painter.drawEllipse(point.point, 3, 3);
        painter.drawText(point.point, QString::number(point.originalIndex + 1));
    }
}
void ConvexHull::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) { // ����Ƿ���������
         // Ϊ�µ���ĵ����һ��ԭʼ���
        int newIndex = pointsWithIndex.size(); // ��ȡ��ǰ��ı�ţ��������˳��

        // ���� PointWithIndex����������ӵ� pointsWithIndex ��
        pointsWithIndex.append(PointWithIndex(event->pos(), newIndex));

        update(); // �����ػ�
    }
}
QString ConvexHull::processLoadPoints(const QString& fileName)
{
    pointsWithIndex.clear();  // ���֮ǰ�ĵ�
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, "����", "�޷����ļ�");
        return "";
    }

    QTextStream in(&file);
    int index = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList coordinates = line.split(" ");
        if (coordinates.size() == 2) {
            int x = coordinates[0].toInt();
            int y = coordinates[1].toInt();
            pointsWithIndex.append(PointWithIndex(QPoint(x, y), index++));  // ����ԭʼ��ͱ��
            qDebug() << "Loaded point:" << x << y;  // ���������Ϣ
        }
    }
    file.close();
    this->repaint();  // ������ʾ
    return fileName;
}
void ConvexHull::on_Load_clicked()
{
    QString str= QFileDialog::getOpenFileName(this, "���ص�����", "", "Text Files (*.txt)");
   QString pointsStr = processLoadPoints(str);
    if (!pointsStr.isEmpty()) {
        QMessageBox::information(this, "��ʾ", "��������سɹ�");
    }
}
void ConvexHull::on_Export_clicked()
{
    if (points.isEmpty()) {
        QMessageBox::information(this, "��ʾ", "û�е���Ե���");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "����͹��������", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const auto& point : pointsWithIndex) {
                out << point.point.x() << " " << point.point.y() << "\n";
            }
            file.close();
            QMessageBox::information(this, "��ʾ", "͹�������굼���ɹ�");
        }
    }
}
void ConvexHull::on_Clear_clicked()
{
    pointsWithIndex.clear();
    convexHull.clear(); // ���͹����
    lineEditPoiCount->clear();
    lineEditPoiCoords->clear();
    this->update();
}

void ConvexHull::on_Start_clicked() 
{
    if (pointsWithIndex.isEmpty()) {
        QMessageBox::information(this, "��ʾ", "������Ļ�ϻ��Ƶ����ص�");
    }
    else {
        drawConvexHull();
        displayConvexHullPoints();
    }
}
void ConvexHull::drawConvexHull() {
    if (pointsWithIndex.size() < 3) {
        QMessageBox::information(this, "��ʾ", "������Ҫ3����������͹��");
        return;
    }

    // ����͹��
    convexHull = computeConvexHull(pointsWithIndex);

   
}
bool ConvexHull::isClockwise(const QPoint& p1, const QPoint& p2, const QPoint& p3)
{
    return (p2.x() - p1.x()) * (p3.y() - p1.y()) - (p2.y() - p1.y()) * (p3.x() - p1.x()) > 0;
}

void ConvexHull::displayConvexHullPoints()
{
    QStringList coordStrings;
    for (int i = 0; i < convexHull.size(); ++i) {
        coordStrings.append(QString("Point %1").arg(convexHull[i].originalIndex + 1));  // ���Ŵ� 1 ��ʼ
    }
    lineEditPoiCount->setText(QString::number(convexHull.size()));
    lineEditPoiCoords->setText(coordStrings.join("; "));
}

QVector<PointWithIndex> ConvexHull::computeConvexHull(const QVector<PointWithIndex>& pointsWithIndex)
{
    if (pointsWithIndex.size() < 3) return convexHull;

    // Graham scan algorithm
     // �ҵ���͵ĵ�
    QPoint start = pointsWithIndex[0].point;
    for (const PointWithIndex& point : pointsWithIndex) {
        if (point.point.y() < start.y() || (point.point.y() == start.y() && point.point.x() < start.x())) {
            start = point.point;
        }
    }

    // ��������
    QVector<PointWithIndex> sortedPoints = pointsWithIndex;
    std::sort(sortedPoints.begin(), sortedPoints.end(), [this, start](const PointWithIndex& a, const PointWithIndex& b) {
        return compare(a.point, b.point, start);
        });


    // ʹ��ջ����͹��
   
    convexHull.push_back(sortedPoints[0]);
    convexHull.push_back(sortedPoints[1]);

    for (int i = 2; i < sortedPoints.size(); ++i) {
        while (convexHull.size() > 1 && crossProduct(convexHull[convexHull.size() - 2].point, convexHull.back().point, sortedPoints[i].point) <= 0) {
            convexHull.pop_back();
        }
        convexHull.push_back(sortedPoints[i]);
    }

    return convexHull;
  
}
