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

    static bool initStyle(); //����Ĭ����ʽ
    static QgsStyle* getStyle(); // ��ȡ���ſ�
    static QgsSymbol* getSymbol(std::string name);  //ͨ�����ƻ�ȡ����
    static QgsSymbol* getSymbol(std::string name, const QColor& color); //����ɫ����


private:
    static QgsStyle m_style; // ��̬��Ա����
};
