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
    // ������������
}

void LayerPropertiesWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    // ����һ��ˮƽ�ָ���
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    m_propertyList = new QListWidget();
    m_mainLayout->addWidget(m_propertyList);

    m_stackedWidget = new QStackedWidget();
    m_mainLayout->addWidget(m_stackedWidget);

    // ���͸���ȵ�������
    m_transparencySlider = new QSlider(Qt::Horizontal);
    m_transparencySlider->setMinimum(0);
    m_transparencySlider->setMaximum(100);
    // ����ͼ�����ͳ�ʼ������ֵ
    if (QgsVectorLayer* vectorLayer = dynamic_cast<QgsVectorLayer*>(m_layer))
    {
        m_transparencySlider->setValue(static_cast<int>((1.0 - vectorLayer->opacity()) * 100)); // ת��Ϊ 0-100 �ķ�Χ
    }
    else if (QgsRasterLayer* rasterLayer = dynamic_cast<QgsRasterLayer*>(m_layer))
    {
        // դ��ͼ�㴦���߼�
        qDebug() << "Layer is a raster layer.";
        // ��ȡդ��ͼ��͸����
        double rasterOpacity = 1.0 - rasterLayer->renderer()->opacity(); // ת��Ϊ͸����ֵ
        m_transparencySlider->setValue(static_cast<int>(rasterOpacity * 100));
    }
    else
    {
        m_transparencySlider->setValue(0); // Ĭ��ֵ
    }

    m_stackedWidget->addWidget(m_transparencySlider);

    // ��� Apply ��ť
    m_applyButton = new QPushButton("Apply");
    connect(m_applyButton, &QPushButton::clicked, this, &LayerPropertiesWidget::applyChanges);
    m_mainLayout->addWidget(m_applyButton);
  
    
    m_mainLayout->addWidget(splitter);
 
   

    // ��ʼ�������б�
    m_propertyList->addItem("Symbolization");
    m_propertyList->setCurrentRow(0);
    splitter->addWidget(m_propertyList);
    // ���öԻ������
    setWindowTitle("ͼ������");

    // ���ó�ʼ��ʾ
    m_stackedWidget->setCurrentIndex(0);
}

void LayerPropertiesWidget::applyChanges()
{
    int transparencyValue = m_transparencySlider->value();

    // ����ͼ������Ӧ��͸����
    if (QgsVectorLayer* vectorLayer = dynamic_cast<QgsVectorLayer*>(m_layer))
    {
        vectorLayer->setOpacity(1.0-transparencyValue / 100.0); // ת��Ϊ 0.0-1.0 �ķ�Χ
        vectorLayer->triggerRepaint(); // ˢ��ͼ��
    }
    else if (QgsRasterLayer* rasterLayer = dynamic_cast<QgsRasterLayer*>(m_layer))
    {
        rasterLayer->renderer()->setOpacity(1.0 - transparencyValue / 100.0); // ֱ��ʹ������ֵ
        rasterLayer->triggerRepaint(); // ˢ��ͼ��
    }
    else
    {
        qDebug() << "Layer type not supported for transparency adjustment.";
    }

    /*emit layerUpdated(m_layer);*/
}