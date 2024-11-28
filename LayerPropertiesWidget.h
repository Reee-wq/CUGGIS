#pragma once
#ifndef LAYERPROPERTIESWIDGET_H
#define LAYERPROPERTIESWIDGET_H

#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QSlider>
#include <QPushButton>
#include "qgsmaplayer.h"
#include "qgsmaplayerrenderer.h"

class LayerPropertiesWidget : public QDialog
{
    Q_OBJECT

public:
    LayerPropertiesWidget(QgsMapLayer* layer, QWidget* parent = nullptr);
    ~LayerPropertiesWidget();

private slots:
    void applyChanges();

private:
    QgsMapLayer* m_layer;
    QVBoxLayout* m_mainLayout;
    QListWidget* m_propertyList;
    QStackedWidget* m_stackedWidget;
    QSlider* m_transparencySlider;
    QPushButton* m_applyButton;


    void setupUI();
};

#endif // LAYERPROPERTIESWIDGET_H