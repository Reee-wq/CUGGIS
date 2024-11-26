//#define _USE_MATH_DEFINES
#include <QtWidgets/QApplication>
#include <qgsapplication.h>

#include "CUGGIS.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QgsApplication::setPrefixPath("D:\osgeo4w-qgis-dev\QGIS-ltr", true); // !!!这个路径参数需要自行修改
    QgsApplication::init();
    QgsApplication::initQgis();
    CUGGIS w;
    w.show();
    return a.exec();
}
