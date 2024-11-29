#include "Buffer.h"
#include <QMessageBox>
#include <qgsprocessing.h>

#include <QgsApplication.h>
#include <QgsProcessing.h>
#include <QgsProcessingContext.h>
#include <QgsProcessingFeedback.h>
#include <QgsProcessingRegistry.h>
#include "gdal.h"


Buffer::Buffer(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

}

Buffer::~Buffer()
{

}

//选择shp文件
void Buffer::on_pushButtonInput_clicked() {
    // 弹出文件选择对话框
    inputFilePath = QFileDialog::getOpenFileName(this, "select a SHP file", "", "Shapefiles (*.shp)");
    // 将选择的文件路径显示在 lineEditInput 中
    if (!inputFilePath.isEmpty()) {
        ui.lineEditInput->setText(inputFilePath);
    }
}

// 选择输出文件夹
void Buffer::on_pushButtonOutput_clicked() {
    // 弹出文件夹选择对话框
    outputFilePath = QFileDialog::getExistingDirectory(this, "select a folder", "");
    // 将选择的文件夹路径显示在 lineEditOutput 中
    if (!outputFilePath.isEmpty()) {
        ui.lineEditOutput->setText(outputFilePath);
    }
}

// 进行缓冲区分析
void Buffer::on_pushButtonOK_clicked() {
    // 获取缓冲区文件名和半径
    outputFileName = ui.lineEditFilename->text();
    QString radiusText = ui.lineEditRadius->text();

    // 检查输入文件路径和输出文件路径是否有效
    if (inputFilePath.isEmpty()) { // 检查是否选择了输入文件
        QMessageBox::warning(this, "Input Error", "Please select an input SHP file.");
        return;
    }
    if (outputFilePath.isEmpty()) { // 检查是否选择了输出路径
        QMessageBox::warning(this, "Output Error", "Please select an output directory.");
        return;
    }
    if (outputFileName.isEmpty()) { // 检查是否输入了输出文件名
        QMessageBox::warning(this, "File Name Error", "Please provide a file name for the output.");
        return;
    }
    if (radiusText.isEmpty()) { // 检查是否输入了缓冲区半径
        QMessageBox::warning(this, "Radius Error", "Please provide a buffer radius.");
        return;
    }

    // 转换缓冲区半径为浮点数
    bool radiusOk;
    bufferRadius = radiusText.toFloat(&radiusOk);
    if (!radiusOk || bufferRadius <= 0) {
        QMessageBox::warning(this, "Radius Error", "Please provide a valid positive buffer radius.");
        return;
    }

    // 使用 OGR 驱动加载矢量文件
    QgsVectorLayer* layer = new QgsVectorLayer(inputFilePath, "Input Layer", "ogr");
    if (!layer->isValid()) {
        QMessageBox::critical(this, "Layer Error", "Failed to load the input SHP file.");
        return;
    }

    // 创建处理上下文和反馈对象
    QgsProcessingContext context;
    QgsProcessingFeedback feedback;

    // 修复和重新投影几何
    QVariantMap fixParams, reprojParams, bufferParams; // 定义参数映射
    fixParams.insert("INPUT", inputFilePath);          // 设置输入文件路径
    fixParams.insert("OUTPUT", "memory:");             // 输出到内存（临时图层）

    // 调用几何修复算法
    auto* fixAlg = QgsApplication::processingRegistry()->algorithmById("native:fixgeometries");
    if (!fixAlg) {
        QMessageBox::critical(this, "Error", "Failed to find fix geometries algorithm.");
        return;
    }
    QVariantMap fixResults = fixAlg->run(fixParams, context, &feedback); // 执行几何修复

    // 设置重新投影参数
    reprojParams.insert("INPUT", fixResults.value("OUTPUT"));  // 修复后的图层作为输入
    reprojParams.insert("TARGET_CRS", "EPSG:32633");           // 设置投影坐标系
    reprojParams.insert("OUTPUT", "memory:");                 // 输出到内存

    // 调用重新投影算法
    auto* reprojAlg = QgsApplication::processingRegistry()->algorithmById("native:reprojectlayer");
    if (!reprojAlg) {
        QMessageBox::critical(this, "Error", "Failed to find reproject layer algorithm.");
        return;
    }
    QVariantMap reprojResults = reprojAlg->run(reprojParams, context, &feedback); // 执行重新投影

    // 设置缓冲区分析参数
    QString outputFilePathWithName = outputFilePath + "/" + ui.lineEditFilename->text() + ".shp"; // 输出文件路径
    bufferParams.insert("INPUT", reprojResults.value("OUTPUT")); // 使用重新投影后的图层作为输入
    bufferParams.insert("DISTANCE", bufferRadius);              // 设置缓冲区半径
    bufferParams.insert("DISSOLVE", true);                      // 设置是否合并缓冲区结果
    bufferParams.insert("OUTPUT", outputFilePathWithName);      // 设置输出文件路径

    // 调用缓冲区算法
    auto* bufferAlg = QgsApplication::processingRegistry()->algorithmById("native:buffer");
    if (!bufferAlg) {
        QMessageBox::critical(this, "Error", "Failed to find buffer algorithm.");
        return;
    }
    bufferAlg->run(bufferParams, context, &feedback); // 执行缓冲区分析

      // 创建 QgsVectorLayer  
    QgsVectorLayer* pointLayer = new QgsVectorLayer(outputFilePathWithName, outputFileName, "ogr");
    if (!pointLayer->isValid()) {
        QMessageBox::critical(this, "错误", "无法加载 Shapefile 图层。");
        return;
    }
    //将转换后的shp文件在图层上
    QgsProject::instance()->addMapLayer(pointLayer);

    // 分析成功，显示成功消息
    QMessageBox::information(this, "Success", "Buffer analysis completed successfully.\nOutput file: " + outputFilePathWithName);
}
