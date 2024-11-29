#include "StyleManager.h"

// 必须定义静态成员变量
QgsStyle StyleManager::m_style;

StyleManager::StyleManager()
{
}

StyleManager::~StyleManager()
{
}

bool StyleManager::initStyle() // 加载QGIS默认符号库
{
    sqlite3_initialize();
    return m_style.load(QgsApplication::userStylePath());
}

QgsStyle* StyleManager::getStyle() // 获取符号库
{
    if (m_style.symbolCount() == 0) // 如果未设置符号，则加载默认样式
    {
        initStyle();
    }
    return &m_style;
}

QgsSymbol* StyleManager::getSymbol(std::string name) // 根据名称获取符号
{
    if (m_style.symbolCount() == 0)
    {
        initStyle();
    }
    return m_style.symbol(QString::fromLocal8Bit(name.c_str()));
}

QgsSymbol* StyleManager::getSymbol(std::string name, const QColor& color) // 修改颜色符号
{
    if (m_style.symbolCount() == 0)
    {
        initStyle();
    }
    QgsSymbol* oriSymbol = m_style.symbol(QString::fromLocal8Bit(name.c_str())); // 获取原符号
    if (oriSymbol == nullptr)
    {
        return nullptr;
    }
    QgsSymbol* symbol = oriSymbol->clone(); // 克隆原符号
    QgsSymbol::SymbolType qgsType = symbol->type(); // 获取符号类型

    if (qgsType != QgsSymbol::SymbolType::Line)
    {
        if (qgsType == QgsSymbol::SymbolType::Marker) // 点符号
        {
            QgsSimpleMarkerSymbolLayer* simplelayer = new QgsSimpleMarkerSymbolLayer();
            simplelayer->setColor(color);
            simplelayer->setStrokeStyle(Qt::PenStyle::NoPen);
            symbol->insertSymbolLayer(0, simplelayer);
        }
        else if (qgsType == QgsSymbol::SymbolType::Fill) // 面符号
        {
            QgsSimpleFillSymbolLayer* simplelayer = new QgsSimpleFillSymbolLayer();
            simplelayer->setColor(color);
            simplelayer->setStrokeStyle(Qt::PenStyle::NoPen);
            symbol->insertSymbolLayer(0, simplelayer);
        }
        QgsSimpleLineSymbolLayer* lineLayer = new QgsSimpleLineSymbolLayer();  // 线符号
        lineLayer->setColor(color);
        lineLayer->setWidth(1);
        symbol->insertSymbolLayer(0, lineLayer);
    }

    return symbol;
}
