#ifndef MYDEFAULTMAPTOOL_H
#define MYDEFAULTMAPTOOL_H

#include <qgsmaptool.h>

class MyDefaultMapTool : public QgsMapTool
{
public:
    explicit MyDefaultMapTool(QgsMapCanvas* canvas);
    // ������ڴ�����Զ������Ϊ��������ΪĬ�Ϲ���
};

#endif // MYDEFAULTMAPTOOL_H
