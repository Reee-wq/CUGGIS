#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_ConvexHull.h"

#include <QTextEdit>
#include <QPushButton>
#include <QVector>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

// 结构体用于保存点和原始编号
struct PointWithIndex {
	QPoint point;
	int originalIndex;  // 保存原始编号

	PointWithIndex(const QPoint& pt, int index)
		: point(pt), originalIndex(index) {}
	// 默认构造函数
	PointWithIndex()
		: point(QPoint(0, 0)), originalIndex(0) {}
};
class ConvexHull : public QWidget
{
	Q_OBJECT

public:
	ConvexHull(QWidget *parent = nullptr);
	~ConvexHull();
	// 计算叉积，用于判断点的顺序
	int crossProduct(const QPoint& O, const QPoint& A, const QPoint& B);
	// 计算两点之间的距离的平方，用于比较
	int distanceSquared(const QPoint& A, const QPoint& B);
	// 比较函数，用于极角排序
	bool compare(const QPoint& a, const QPoint& b, const QPoint& start);
protected:
	void paintEvent(QPaintEvent* event) override;  // 声明 paintEvent() 函数
	 void mousePressEvent(QMouseEvent *event) override; // 重写
private:
	Ui::ConvexHullClass ui;
	QVector<QPoint> points;
	QVector<PointWithIndex> convexHull;
	QLineEdit* lineEditPoiCount;
	QLineEdit* lineEditPoiCoords;
	QVector<PointWithIndex> pointsWithIndex;  // 用于存储点和原始编号
private slots:
	void on_Start_clicked();
	void on_Load_clicked();
	void on_Export_clicked();
	void on_Clear_clicked();
	QString processLoadPoints(const QString& fileName);
	void drawConvexHull();
	QVector<PointWithIndex> computeConvexHull(const QVector<PointWithIndex>& pointsWithIndex);
	bool isClockwise(const QPoint& p1, const QPoint& p2, const QPoint& p3);
	void displayConvexHullPoints();
};
