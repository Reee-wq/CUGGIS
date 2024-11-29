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
    // ��������ʱ��������ͼ��
    a.setWindowIcon(QIcon("./Resources/CUGGIS.jpg"));
    // ����һ��QPixmap������������ͼƬ
    QPixmap pixmap("./Resources/Start.jpg");
    pixmap = pixmap.scaled(700, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation); // ����ͼƬ��С

   // ����QPixmap���󴴽�һ��QSplashScreen����
    QSplashScreen splash(pixmap);
    splash.show();

    // ����һ��������
    QProgressBar* progressBar = new QProgressBar;
    progressBar->setRange(0, 100); // ���ý������ķ�Χ

    // ����һ����ֱ����
    QVBoxLayout* mainLayout = new QVBoxLayout;

    // ����һ��ˮƽ���֣����ڷ��ý�����
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(progressBar);
    bottomLayout->addStretch(); // ������죬ʹ���������Ҷ���

    // ��ˮƽ������ӵ���ֱ���ֵĵײ�
    mainLayout->addStretch(); // ���һ�����죬ʹ�������ڵײ�
    mainLayout->addLayout(bottomLayout);

    // ���������ø�QSplashScreen
    splash.setLayout(mainLayout);

    // ʹ��������ʾ��������ʱ������Ӧ���������¼�
    a.processEvents();
    // ��ӦͼƬ���
    progressBar->setMinimumWidth(splash.width());

    // ģ��һЩ��ʱ�ĳ�ʼ������
    for (int i = 0; i <= 100; ++i) {
        // ���½�����
        progressBar->setValue(i);
        a.processEvents(); // �����¼���ȷ���������
        QThread::msleep(0.7); // ģ���ʱ����
    }
    QgsApplication::setPrefixPath("D:/osgeo4w-qgis-dev/QGIS-ltr", true); // !!!���·��������Ҫ�����޸�
    QgsApplication::init();
    QgsApplication::initQgis();
    //���∐ͬѧ�����ṩ
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    // ע�� Native �� GDAL �ṩ��
    QgsApplication::processingRegistry()->addProvider(new QgsNativeAlgorithms());
    CUGGIS w;
  w.setWindowIcon(QIcon("./Resources/CUGGIS.jpg"));
    w.showMaximized();
    // ��ʾ������������ʼ����ɺ󣬽�����������
    splash.finish(&w);
    return a.exec();
}
