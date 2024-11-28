#pragma once
#ifndef VECTORYLAYERPROPERTIESWIDGET_H
#define VECTORYLAYERPROPERTIESWIDGET_H

#include "LayerPropertiesWidget.h"

class VectorLayerPropertiesWidget : public LayerPropertiesWidget
{
    Q_OBJECT

public:
    VectorLayerPropertiesWidget(QgsMapLayer* layer, QWidget* parent = nullptr);
};

#endif // VECTORYLAYERPROPERTIESWIDGET_H