#include "StyleManager.h"

// ���붨�徲̬��Ա����
QgsStyle StyleManager::m_style;

StyleManager::StyleManager()
{
}

StyleManager::~StyleManager()
{
}

bool StyleManager::initStyle() // ����QGISĬ�Ϸ��ſ�
{
    sqlite3_initialize();
    return m_style.load(QgsApplication::userStylePath());
}

QgsStyle* StyleManager::getStyle() // ��ȡ���ſ�
{
    if (m_style.symbolCount() == 0) // ���δ���÷��ţ������Ĭ����ʽ
    {
        initStyle();
    }
    return &m_style;
}

QgsSymbol* StyleManager::getSymbol(std::string name) // �������ƻ�ȡ����
{
    if (m_style.symbolCount() == 0)
    {
        initStyle();
    }
    return m_style.symbol(QString::fromLocal8Bit(name.c_str()));
}

QgsSymbol* StyleManager::getSymbol(std::string name, const QColor& color) // �޸���ɫ����
{
    if (m_style.symbolCount() == 0)
    {
        initStyle();
    }
    QgsSymbol* oriSymbol = m_style.symbol(QString::fromLocal8Bit(name.c_str())); // ��ȡԭ����
    if (oriSymbol == nullptr)
    {
        return nullptr;
    }
    QgsSymbol* symbol = oriSymbol->clone(); // ��¡ԭ����
    QgsSymbol::SymbolType qgsType = symbol->type(); // ��ȡ��������

    if (qgsType != QgsSymbol::SymbolType::Line)
    {
        if (qgsType == QgsSymbol::SymbolType::Marker) // �����
        {
            QgsSimpleMarkerSymbolLayer* simplelayer = new QgsSimpleMarkerSymbolLayer();
            simplelayer->setColor(color);
            simplelayer->setStrokeStyle(Qt::PenStyle::NoPen);
            symbol->insertSymbolLayer(0, simplelayer);
        }
        else if (qgsType == QgsSymbol::SymbolType::Fill) // �����
        {
            QgsSimpleFillSymbolLayer* simplelayer = new QgsSimpleFillSymbolLayer();
            simplelayer->setColor(color);
            simplelayer->setStrokeStyle(Qt::PenStyle::NoPen);
            symbol->insertSymbolLayer(0, simplelayer);
        }
        QgsSimpleLineSymbolLayer* lineLayer = new QgsSimpleLineSymbolLayer();  // �߷���
        lineLayer->setColor(color);
        lineLayer->setWidth(1);
        symbol->insertSymbolLayer(0, lineLayer);
    }

    return symbol;
}
