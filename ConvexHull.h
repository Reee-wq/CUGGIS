#pragma once

#include <QWidget>
#include "ui_ConvexHull.h"

#include <QTextEdit>
#include <QPushButton>
#include <QVector>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

class ConvexHull : public QWidget
{
	Q_OBJECT

public:
	ConvexHull(QWidget *parent = nullptr);
	~ConvexHull();

private:
	Ui::ConvexHullClass ui;
	QVector<QPoint> points;
	QWidget* widget;
private slots:
	void on_pushButton_start_clicked();
	void on_pushButton_load_clicked();
	void on_pushButton_export_clicked();
	void on_pushButton_clear_clicked();
	QString processLoadPoints(QString& pointsStr);
	void drawConvexHull();
	QVector<QPoint> computeConvexHull(const QVector<QPoint>& points);
	bool isClockwise(const QPoint& p1, const QPoint& p2, const QPoint& p3);
};
