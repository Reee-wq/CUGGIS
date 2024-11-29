//#define _USE_MATH_DEFINES
#pragma execution_character_set("utf-8")
#include <QtWidgets/QApplication>
#include "CUGGIS.h"
#include <QgsApplication.h>
#include <QgsProcessingRegistry.h>
#include <QgsNativeAlgorithms.h>
#include <QgsProviderRegistry.h>
#include <QgsGdalProvider.h>

//#include "CUGGIS.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QgsApplication::setPrefixPath("D:\OSGeo4W\osgeo4w-qgis-dev\osgeo4w-qgis-dev\QGIS-ltr", true); // !!!这个路径参数需要自行修改
    QgsApplication::init();
    QgsApplication::initQgis();
    // 注册 Native 和 GDAL 提供者
    QgsApplication::processingRegistry()->addProvider(new QgsNativeAlgorithms());
    CUGGIS w;
    w.show();
    return a.exec();
}
