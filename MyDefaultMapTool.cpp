#include "MyDefaultMapTool.h"

MyDefaultMapTool::MyDefaultMapTool(QgsMapCanvas* canvas)
    : QgsMapTool(canvas)
{
    setCursor(Qt::ArrowCursor); // ����Ĭ�Ϲ��Ϊ��ͷ
}
