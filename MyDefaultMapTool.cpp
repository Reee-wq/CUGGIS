#include "MyDefaultMapTool.h"

MyDefaultMapTool::MyDefaultMapTool(QgsMapCanvas* canvas)
    : QgsMapTool(canvas)
{
    setCursor(Qt::ArrowCursor); // 设置默认光标为箭头
}
