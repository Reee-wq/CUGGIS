#ifndef MYDEFAULTMAPTOOL_H
#define MYDEFAULTMAPTOOL_H

#include <qgsmaptool.h>

class MyDefaultMapTool : public QgsMapTool
{
public:
    explicit MyDefaultMapTool(QgsMapCanvas* canvas);
    // 你可以在此添加自定义的行为或留空作为默认工具
};

#endif // MYDEFAULTMAPTOOL_H
