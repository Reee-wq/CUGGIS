#pragma execution_character_set("utf-8")
#include "LayerPropertiesWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include "qgsmaplayerrenderer.h"
#include "qgsvectorlayer.h"
#include "qgsrasterlayer.h"
#include "qgsrastertransparency.h"
#include "qgsrasterrenderer.h"

LayerPropertiesWidget::LayerPropertiesWidget(QgsMapLayer* layer, QWidget* parent)
    : QDialog(parent), m_layer(layer)
{
    setupUI();
}

LayerPropertiesWidget::~LayerPropertiesWidget()
{
    // 无需特殊清理
}

void LayerPropertiesWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    // 创建一个水平分割器
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    m_propertyList = new QListWidget();
    m_mainLayout->addWidget(m_propertyList);

    m_stackedWidget = new QStackedWidget();
    m_mainLayout->addWidget(m_stackedWidget);

    // 添加透明度调整滑块
    m_transparencySlider = new QSlider(Qt::Horizontal);
    m_transparencySlider->setMinimum(0);
    m_transparencySlider->setMaximum(100);
    // 根据图层类型初始化滑块值
    if (QgsVectorLayer* vectorLayer = dynamic_cast<QgsVectorLayer*>(m_layer))
    {
        m_transparencySlider->setValue(static_cast<int>((1.0 - vectorLayer->opacity()) * 100)); // 转换为 0-100 的范围
    }
    else if (QgsRasterLayer* rasterLayer = dynamic_cast<QgsRasterLayer*>(m_layer))
    {
        // 栅格图层处理逻辑
        qDebug() << "Layer is a raster layer.";
        // 获取栅格图层透明度
        double rasterOpacity = 1.0 - rasterLayer->renderer()->opacity(); // 转换为透明度值
        m_transparencySlider->setValue(static_cast<int>(rasterOpacity * 100));
    }
    else
    {
        m_transparencySlider->setValue(0); // 默认值
    }

    m_stackedWidget->addWidget(m_transparencySlider);

    // 添加 Apply 按钮
    m_applyButton = new QPushButton("Apply");
    connect(m_applyButton, &QPushButton::clicked, this, &LayerPropertiesWidget::applyChanges);
    m_mainLayout->addWidget(m_applyButton);
  
    
    m_mainLayout->addWidget(splitter);
 
   

    // 初始化属性列表
    m_propertyList->addItem("Symbolization");
    m_propertyList->setCurrentRow(0);
    splitter->addWidget(m_propertyList);
    // 设置对话框标题
    setWindowTitle("图层属性");

    // 设置初始显示
    m_stackedWidget->setCurrentIndex(0);
}

void LayerPropertiesWidget::applyChanges()
{
    int transparencyValue = m_transparencySlider->value();

    // 根据图层类型应用透明度
    if (QgsVectorLayer* vectorLayer = dynamic_cast<QgsVectorLayer*>(m_layer))
    {
        vectorLayer->setOpacity(1.0-transparencyValue / 100.0); // 转换为 0.0-1.0 的范围
        vectorLayer->triggerRepaint(); // 刷新图层
    }
    else if (QgsRasterLayer* rasterLayer = dynamic_cast<QgsRasterLayer*>(m_layer))
    {
        rasterLayer->renderer()->setOpacity(1.0 - transparencyValue / 100.0); // 直接使用整数值
        rasterLayer->triggerRepaint(); // 刷新图层
    }
    else
    {
        qDebug() << "Layer type not supported for transparency adjustment.";
    }

    /*emit layerUpdated(m_layer);*/
}