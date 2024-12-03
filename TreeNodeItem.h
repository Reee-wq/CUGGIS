#pragma once
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QBrush>
#include <qgraphicsitem.h>
#include "decisionstructure.h"
class TreeNodeItem :
    public QGraphicsItem
{
public:
    TreeNodeItem(const QString& text, const QRectF& rect)
        : m_text(text), m_rect(rect) {}

    QRectF boundingRect() const override {
        return m_rect;
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setBrush(QBrush(Qt::lightGray));
        painter->setPen(QPen(Qt::black));
        painter->drawEllipse(m_rect);  // ���ڵ�
        painter->drawText(m_rect, Qt::AlignCenter, m_text);  // ��ʾ�ڵ��ı�
    }

    QString m_text;
    QRectF m_rect;
   
};

