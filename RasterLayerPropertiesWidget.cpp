#include "RasterLayerPropertiesWidget.h"

RasterLayerPropertiesWidget::RasterLayerPropertiesWidget(QgsMapLayer* layer, QWidget* parent)
    : LayerPropertiesWidget(layer, parent)
{
    // 可以在这里添加栅格图层特有的一些配置
    // 例如，添加更多的属性列表项
}