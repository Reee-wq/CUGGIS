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
    lineEditPoiCount = ui.lineEditPoiCount; // 假设您的lineEditPoiCount在ui文件中的对象名是lineEditPoiCount
    lineEditPoiCoords = ui.lineEditPoiCoords; // 假设您的lineEditPoiCoords在ui文件中的对象名是lineEditPoiCoords
    // 设置lineEdit为只读
    lineEditPoiCount->setReadOnly(true);
    lineEditPoiCoords->setReadOnly(true);
    //// 设置sizePolicy
    //lineEditPoiCount->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    //lineEditPoiCoords->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    //// 获取QPushButton控件
    //QPushButton* startButton = ui.Start;
    //QPushButton* exportButton = ui.Export;
    //QPushButton* clearButton = ui.Clear;
    //startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    //exportButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    //clearButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    adjustSize();  // 使主窗口大小自适应内容大小
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
    painter.setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿效果

    // 设置点的颜色和大小
    painter.setPen(Qt::black);  // 画笔颜色
    painter.setBrush(Qt::green); // 填充颜色

    // 绘制所有点
    for (const PointWithIndex& point : pointsWithIndex) {
        painter.drawEllipse(point.point, 3, 3);  // 绘制半径为3的圆形点
    }
    // 如果已经计算了凸包，绘制凸包
    if (!convexHull.isEmpty()) {
        painter.setPen(Qt::blue);  // 凸包的颜色
        for (int i = 0; i < convexHull.size(); ++i) {
            int j = (i + 1) % convexHull.size();  // 循环连接最后一个点和第一个点
            painter.drawLine(convexHull[i].point, convexHull[j].point);
        }
    }
    // 绘制所有点和编号
    painter.setPen(Qt::red);
    for (const PointWithIndex& point : pointsWithIndex) {
        painter.drawEllipse(point.point, 3, 3);
        painter.drawText(point.point, QString::number(point.originalIndex + 1));
    }
}
void ConvexHull::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) { // 检测是否是左键点击
         // 为新点击的点分配一个原始编号
        int newIndex = pointsWithIndex.size(); // 获取当前点的编号（即点击的顺序）

        // 创建 PointWithIndex，并将其添加到 pointsWithIndex 中
        pointsWithIndex.append(PointWithIndex(event->pos(), newIndex));

        update(); // 请求重绘
    }
}
QString ConvexHull::processLoadPoints(const QString& fileName)
{
    pointsWithIndex.clear();  // 清空之前的点
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, "错误", "无法打开文件");
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
            pointsWithIndex.append(PointWithIndex(QPoint(x, y), index++));  // 保存原始点和编号
            qDebug() << "Loaded point:" << x << y;  // 输出调试信息
        }
    }
    file.close();
    this->repaint();  // 更新显示
    return fileName;
}
void ConvexHull::on_Load_clicked()
{
    QString str= QFileDialog::getOpenFileName(this, "加载点坐标", "", "Text Files (*.txt)");
   QString pointsStr = processLoadPoints(str);
    if (!pointsStr.isEmpty()) {
        QMessageBox::information(this, "提示", "点坐标加载成功");
    }
}
void ConvexHull::on_Export_clicked()
{
    if (points.isEmpty()) {
        QMessageBox::information(this, "提示", "没有点可以导出");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "导出凸包点坐标", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const auto& point : pointsWithIndex) {
                out << point.point.x() << " " << point.point.y() << "\n";
            }
            file.close();
            QMessageBox::information(this, "提示", "凸包点坐标导出成功");
        }
    }
}
void ConvexHull::on_Clear_clicked()
{
    pointsWithIndex.clear();
    convexHull.clear(); // 清除凸包点
    lineEditPoiCount->clear();
    lineEditPoiCoords->clear();
    this->update();
}

void ConvexHull::on_Start_clicked() 
{
    if (pointsWithIndex.isEmpty()) {
        QMessageBox::information(this, "提示", "请在屏幕上绘制点或加载点");
    }
    else {
        drawConvexHull();
        displayConvexHullPoints();
    }
}
void ConvexHull::drawConvexHull() {
    if (pointsWithIndex.size() < 3) {
        QMessageBox::information(this, "提示", "至少需要3个点来生成凸包");
        return;
    }

    // 计算凸包
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
        coordStrings.append(QString("Point %1").arg(convexHull[i].originalIndex + 1));  // 点编号从 1 开始
    }
    lineEditPoiCount->setText(QString::number(convexHull.size()));
    lineEditPoiCoords->setText(coordStrings.join("; "));
}

QVector<PointWithIndex> ConvexHull::computeConvexHull(const QVector<PointWithIndex>& pointsWithIndex)
{
    if (pointsWithIndex.size() < 3) return convexHull;

    // Graham scan algorithm
     // 找到最低的点
    QPoint start = pointsWithIndex[0].point;
    for (const PointWithIndex& point : pointsWithIndex) {
        if (point.point.y() < start.y() || (point.point.y() == start.y() && point.point.x() < start.x())) {
            start = point.point;
        }
    }

    // 极角排序
    QVector<PointWithIndex> sortedPoints = pointsWithIndex;
    std::sort(sortedPoints.begin(), sortedPoints.end(), [this, start](const PointWithIndex& a, const PointWithIndex& b) {
        return compare(a.point, b.point, start);
        });


    // 使用栈构建凸包
   
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
