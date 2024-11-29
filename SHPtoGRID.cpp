#include "SHPtoGRID.h"
#include <QMessageBox>
#include <qgsprocessing.h>
#include <QgsApplication.h>
#include <QgsProcessing.h>
#include <QgsProcessingContext.h>
#include <QgsProcessingFeedback.h>
#include <QgsProcessingRegistry.h>
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
//#include "CUGGIS.h"
#include <QgsRasterLayer.h>

SHPtoGRID::SHPtoGRID(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

SHPtoGRID::~SHPtoGRID()
{}

//选择shp文件
void SHPtoGRID::on_pushButtonInput_clicked() {
    // 弹出文件选择对话框
    inputFilePath = QFileDialog::getOpenFileName(this, "select a SHP file", "", "Shapefiles (*.shp)");
    // 将选择的文件路径显示在 lineEditInput 中
    if (!inputFilePath.isEmpty()) {
        ui.lineEditInput->setText(inputFilePath);
    }
}

// 选择输出文件夹
void SHPtoGRID::on_pushButtonOutput_clicked() {
    // 弹出文件夹选择对话框
    outputFilePath = QFileDialog::getExistingDirectory(this, "select a folder", "");
    // 将选择的文件夹路径显示在 lineEditOutput 中
    if (!outputFilePath.isEmpty()) {
        ui.lineEditOutput->setText(outputFilePath);
    }
}

void SHPtoGRID::on_pushButtonOK_clicked() {
    outputFileName = ui.lineEditFilename->text(); // 获取输出文件名

    // 检查输入文件路径和输出文件路径是否有效
    if (inputFilePath.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please select an input SHP file.");
        return;
    }
    if (outputFilePath.isEmpty()) {
        QMessageBox::warning(this, "Output Error", "Please select an output directory.");
        return;
    }
    if (outputFileName.isEmpty()) {
        QMessageBox::warning(this, "File Name Error", "Please provide a file name for the output.");
        return;
    }

    QString rasterOutputPath = outputFilePath + "/" + outputFileName + ".tif"; // 输出栅格路径

    // 检查输出目录是否可写
    QFileInfo outputDirInfo(outputFilePath);
    if (!outputDirInfo.isWritable()) {
        QMessageBox::critical(this, "Permission Error", "Output directory is not writable.");
        return;
    }

    // 检查文件是否已存在，如果已存在，删除它
    QFileInfo rasterFileInfo(rasterOutputPath);
    if (rasterFileInfo.exists()) {
        QFile::remove(rasterOutputPath); // 删除现有文件
    }

    // 初始化 GDAL
    GDALAllRegister();
    GDALDataset* poDataset = static_cast<GDALDataset*>(GDALOpenEx(inputFilePath.toStdString().c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
    if (!poDataset) {
        QMessageBox::critical(this, "GDAL Error", "Failed to open input SHP file.");
        return;
    }

    // 获取矢量图层
    OGRLayer* poLayer = poDataset->GetLayer(0);
    if (!poLayer) {
        QMessageBox::critical(this, "Layer Error", "Failed to get layer from SHP file.");
        GDALClose(poDataset);
        return;
    }

    // 获取矢量的范围
    OGREnvelope envelope;
    if (poLayer->GetExtent(&envelope) != OGRERR_NONE) {
        QMessageBox::critical(this, "Extent Error", "Failed to get spatial extent from the input SHP file.");
        GDALClose(poDataset);
        return;
    }

    // 检查范围是否合理
    if (envelope.MinX >= envelope.MaxX || envelope.MinY >= envelope.MaxY) {
        QMessageBox::critical(this, "Extent Error", "Invalid spatial extent. Check the input SHP file.");
        GDALClose(poDataset);
        return;
    }

    // 动态计算像素大小
    double maxResolution = 5000.0; // 最大分辨率限制
    double width = envelope.MaxX - envelope.MinX;
    double height = envelope.MaxY - envelope.MinY;

    double pixelSize = std::max(width, height) / maxResolution;
    qDebug() << "Dynamic Pixel Size: " << pixelSize;

    // 计算栅格宽度和高度
    int rasterWidth = static_cast<int>(std::ceil(width / pixelSize));
    int rasterHeight = static_cast<int>(std::ceil(height / pixelSize));

    qDebug() << "Raster Dimensions: " << rasterWidth << "x" << rasterHeight;

    // 检查栅格尺寸是否合理
    if (rasterWidth <= 0 || rasterHeight <= 0 || rasterWidth > 10000 || rasterHeight > 10000) {
        QMessageBox::critical(this, "Raster Size Error", "Raster dimensions are invalid or too large. Check pixel size or input extent.");
        GDALClose(poDataset);
        return;
    }

    // 确认 GDAL 支持 GTiff 驱动
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (!poDriver) {
        QMessageBox::critical(this, "Driver Error", "GTiff driver not initialized.");
        GDALClose(poDataset);
        return;
    }

    // 创建输出栅格
    char** papszOptions = nullptr;
    GDALDataset* poRasterDS = poDriver->Create(rasterOutputPath.toUtf8().constData(), rasterWidth, rasterHeight, 1, GDT_Byte, papszOptions);
    if (!poRasterDS) {
        QMessageBox::critical(this, "Output Error", "Failed to create output raster file.");
        GDALClose(poDataset);
        return;
    }

    // 设置地理变换参数
    double adfGeoTransform[6] = { envelope.MinX, pixelSize, 0, envelope.MaxY, 0, -pixelSize };
    poRasterDS->SetGeoTransform(adfGeoTransform);

    // 设置坐标参考系
    char* pszWKT = nullptr;
    poLayer->GetSpatialRef()->exportToWkt(&pszWKT);
    poRasterDS->SetProjection(pszWKT);
    CPLFree(pszWKT);

    // 定义并初始化 pahLayers
    OGRLayerH pahLayers[] = { static_cast<OGRLayerH>(poLayer) }; // 将 OGRLayer 转换为 OGRLayerH 类型
    int bandList[1] = { 1 }; // 波段列表，操作第一个波段


    // 栅格化矢量图层，特别处理点数据
    if (poLayer->GetGeomType() == wkbPoint || poLayer->GetGeomType() == wkbMultiPoint) {
        // 点数据的特殊处理
        double burnValue = 1.0;  // 填充值
        double pointRadius = pixelSize * 900;//半径，设置为像素大小的两倍

        char** rasterizeOptions = nullptr;
        rasterizeOptions = CSLAddString(rasterizeOptions, "-burn");
        rasterizeOptions = CSLAddString(rasterizeOptions, std::to_string(burnValue).c_str());
        rasterizeOptions = CSLAddString(rasterizeOptions, "-at");

        // 栅格化点图层
        CPLErr eErr = GDALRasterizeLayers(
            poRasterDS,                   // 输出栅格数据集
            1,                            // 波段数量
            bandList,                     // 波段列表
            1,                            // 图层数量
            pahLayers,                    // 矢量图层数组
            nullptr,                      // 变换函数
            nullptr,                      // 变换参数
            &burnValue,                   // 填充值数组
            rasterizeOptions,             // 栅格化选项
            GDALDummyProgress,            // 进度函数
            nullptr                       // 进度参数
        );

        // 释放选项内存
        CSLDestroy(rasterizeOptions);

        if (eErr != CE_None) {
            QMessageBox::critical(this, "Rasterize Error", "Failed to rasterize point vector data.");
            GDALClose(poRasterDS);
            GDALClose(poDataset);
            return;
        }
    }
    else {
        // 面或线数据的标准处理
        double burnValue = 1.0;           // 填充值

        CPLErr eErr = GDALRasterizeLayers(
            poRasterDS,                   // 输出栅格数据集
            1,                            // 波段数量
            bandList,                     // 波段列表
            1,                            // 图层数量
            pahLayers,                    // 矢量图层数组
            nullptr,                      // 变换函数
            nullptr,                      // 变换参数
            &burnValue,                   // 填充值数组
            nullptr,                      // 栅格化选项
            GDALDummyProgress,            // 进度函数
            nullptr                       // 进度参数
        );

        if (eErr != CE_None) {
            QMessageBox::critical(this, "Rasterize Error", "Failed to rasterize vector data.");
            GDALClose(poRasterDS);
            GDALClose(poDataset);
            return;
        }
    }
    // 关闭数据集
    GDALClose(poRasterDS);
    GDALClose(poDataset);

    //emit rasterGenerated(rasterOutputPath); // 栅格化完成后发送信号
    // 创建 QgsRasterLayer
    QgsRasterLayer* rasterLayer = new QgsRasterLayer(rasterOutputPath, outputFileName);
    if (!rasterLayer->isValid()) {
        QMessageBox::critical(this, "错误", "cannot load raster");
        return;
    }
    // 将加载的栅格图层添加到项目中
    QgsProject::instance()->addMapLayer(rasterLayer);


    QMessageBox::information(this, "Success", "Vector data rasterized successfully.\nOutput file: " + rasterOutputPath);
}
