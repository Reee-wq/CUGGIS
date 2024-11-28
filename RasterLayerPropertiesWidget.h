#pragma once
#ifndef RASTERYLAYERPROPERTIESWIDGET_H
#define RASTERYLAYERPROPERTIESWIDGET_H

#include "LayerPropertiesWidget.h"

class RasterLayerPropertiesWidget : public LayerPropertiesWidget
{
    Q_OBJECT

public:
    RasterLayerPropertiesWidget(QgsMapLayer* layer, QWidget* parent = nullptr);
};

#endif // RASTERYLAYERPROPERTIESWIDGET_H