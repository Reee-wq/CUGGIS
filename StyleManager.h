#pragma once
#include "qgsstyle.h"
#include "qgssymbol.h"
#include "qgsapplication.h"
#include "qgssymbollayer.h"
#include "qgsmarkersymbollayer.h"
#include "qgsfillsymbollayer.h"
#include "qgsLineSymbolLayer.h"

class StyleManager
{
public:
    StyleManager();
    ~StyleManager();

    static bool initStyle(); //加载默认样式
    static QgsStyle* getStyle(); // 获取符号库
    static QgsSymbol* getSymbol(std::string name);  //通过名称获取符号
    static QgsSymbol* getSymbol(std::string name, const QColor& color); //带颜色背景


private:
    static QgsStyle m_style; // 静态成员变量
};
