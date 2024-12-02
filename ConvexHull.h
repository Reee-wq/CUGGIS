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

// �ṹ�����ڱ�����ԭʼ���
struct PointWithIndex {
	QPoint point;
	int originalIndex;  // ����ԭʼ���

	PointWithIndex(const QPoint& pt, int index)
		: point(pt), originalIndex(index) {}
	// Ĭ�Ϲ��캯��
	PointWithIndex()
		: point(QPoint(0, 0)), originalIndex(0) {}
};
class ConvexHull : public QWidget
{
	Q_OBJECT

public:
	ConvexHull(QWidget *parent = nullptr);
	~ConvexHull();
	// �������������жϵ��˳��
	int crossProduct(const QPoint& O, const QPoint& A, const QPoint& B);
	// ��������֮��ľ����ƽ�������ڱȽ�
	int distanceSquared(const QPoint& A, const QPoint& B);
	// �ȽϺ��������ڼ�������
	bool compare(const QPoint& a, const QPoint& b, const QPoint& start);
protected:
	void paintEvent(QPaintEvent* event) override;  // ���� paintEvent() ����
	 void mousePressEvent(QMouseEvent *event) override; // ��д
private:
	Ui::ConvexHullClass ui;
	QVector<QPoint> points;
	QVector<PointWithIndex> convexHull;
	QLineEdit* lineEditPoiCount;
	QLineEdit* lineEditPoiCoords;
	QVector<PointWithIndex> pointsWithIndex;  // ���ڴ洢���ԭʼ���
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
