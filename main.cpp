//#define _USE_MATH_DEFINES
#pragma execution_character_set("utf-8")
#include <QtWidgets/QApplication>
#include "CUGGIS.h"
#include <QgsApplication.h>
#include <QgsProcessingRegistry.h>
#include <QgsNativeAlgorithms.h>
#include <QgsProviderRegistry.h>
#include <QgsGdalProvider.h>
#include <QStyleFactory>
#include <QSplashScreen>
#include <QPixmap>
#include <QProgressBar>
#include <QVBoxLayout>

//#include "CUGGIS.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置运行时的任务栏图标
    a.setWindowIcon(QIcon("./Resources/CUGGIS.jpg"));
    // 创建一个QPixmap对象，设置启动图片
    QPixmap pixmap("./Resources/Start.jpg");
    pixmap = pixmap.scaled(700, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation); // 调整图片大小

   // 利用QPixmap对象创建一个QSplashScreen对象
    QSplashScreen splash(pixmap);
    splash.show();

    // 创建一个进度条
    QProgressBar* progressBar = new QProgressBar;
    progressBar->setRange(0, 100); // 设置进度条的范围

    // 创建一个垂直布局
    QVBoxLayout* mainLayout = new QVBoxLayout;

    // 创建一个水平布局，用于放置进度条
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(progressBar);
    bottomLayout->addStretch(); // 添加拉伸，使进度条靠右对齐

    // 将水平布局添加到垂直布局的底部
    mainLayout->addStretch(); // 添加一个拉伸，使进度条在底部
    mainLayout->addLayout(bottomLayout);

    // 将布局设置给QSplashScreen
    splash.setLayout(mainLayout);

    // 使程序在显示启动画面时仍能响应鼠标等其他事件
    a.processEvents();
    // 适应图片宽度
    progressBar->setMinimumWidth(splash.width());

    // 模拟一些耗时的初始化操作
    for (int i = 0; i <= 100; ++i) {
        // 更新进度条
        progressBar->setValue(i);
        a.processEvents(); // 处理事件，确保界面更新
        QThread::msleep(0.7); // 模拟耗时操作
    }
    QgsApplication::setPrefixPath("D:/osgeo4w-qgis-dev/QGIS-ltr", true); // !!!这个路径参数需要自行修改
    QgsApplication::init();
    QgsApplication::initQgis();
    //覃光垚同学友情提供
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    // 注册 Native 和 GDAL 提供者
    QgsApplication::processingRegistry()->addProvider(new QgsNativeAlgorithms());
    CUGGIS w;
  w.setWindowIcon(QIcon("./Resources/CUGGIS.jpg"));
    w.showMaximized();
    // 表示在主窗体对象初始化完成后，结束启动画面
    splash.finish(&w);
    return a.exec();
}
