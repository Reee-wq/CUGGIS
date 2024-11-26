#include "ConvexHull.h"
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

ConvexHull::ConvexHull(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    widget = new QWidget(this);
    widget->setFixedSize(800, 600);
}

ConvexHull::~ConvexHull()
{}

QString ConvexHull::processLoadPoints(QString& pointsStr)
{
    points.clear();
    QString* string = &pointsStr;
    QTextStream in(string);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList coordinates = line.split(" ");
        if (coordinates.size() == 2) {
            int x = coordinates[0].toInt();
            int y = coordinates[1].toInt();
            points.append(QPoint(x, y));
        }
    }
    widget->update();
    return pointsStr;
}
void ConvexHull::on_pushButton_load_clicked()
{
    QString str= QFileDialog::getOpenFileName(this, "加载点坐标", "", "Text Files (*.txt)");
   QString pointsStr = processLoadPoints(str);
    if (!pointsStr.isEmpty()) {
        QMessageBox::information(this, "提示", "点坐标加载成功");
    }
}
void ConvexHull::on_pushButton_export_clicked()
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
            for (const auto& point : points) {
                out << point.x() << " " << point.y() << "\n";
            }
            file.close();
            QMessageBox::information(this, "提示", "凸包点坐标导出成功");
        }
    }
}
void ConvexHull::on_pushButton_clear_clicked()
{
    points.clear();
    widget->update();
}

void ConvexHull::on_pushButton_start_clicked() 
{
    if (points.isEmpty()) {
        QMessageBox::information(this, "提示", "请在屏幕上绘制点或加载点");
    }
    else {
        drawConvexHull();
    }
}
void ConvexHull::drawConvexHull() {
    if (points.size() < 3) {
        QMessageBox::information(this, "提示", "至少需要3个点来生成凸包");
        return;
    }

    // 计算凸包
    QVector<QPoint> convexHull = computeConvexHull(points);

    // 绘制凸包
    QPainter painter(widget);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::blue);

    for (size_t i = 0; i < convexHull.size(); ++i) {
        painter.drawLine(convexHull[i], convexHull[(i + 1) % convexHull.size()]);
    }

    // 绘制所有点
    for (const auto& point : points) {
        painter.drawPoint(point);
    }
}
bool ConvexHull::isClockwise(const QPoint& p1, const QPoint& p2, const QPoint& p3)
{
    return (p2.x() - p1.x()) * (p3.y() - p1.y()) - (p2.y() - p1.y()) * (p3.x() - p1.x()) > 0;
}

QVector<QPoint> ConvexHull::computeConvexHull(const QVector<QPoint>& points)
{
    if (points.size() < 3) return {};

    // Graham scan algorithm
    std::sort(points.begin(), points.end());

    QVector<QPoint> convexHull;
    convexHull.push_back(points[0]);
    convexHull.push_back(points[1]);

    for (size_t i = 2; i < points.size(); ++i) {
        while (convexHull.size() > 1 && !isClockwise(convexHull[convexHull.size() - 2], convexHull.back(), points[i])) {
            convexHull.pop_back();
        }
        convexHull.push_back(points[i]);
    }

    return convexHull;
}
