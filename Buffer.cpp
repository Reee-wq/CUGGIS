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

//ѡ��shp�ļ�
void Buffer::on_pushButtonInput_clicked() {
    // �����ļ�ѡ��Ի���
    inputFilePath = QFileDialog::getOpenFileName(this, "select a SHP file", "", "Shapefiles (*.shp)");
    // ��ѡ����ļ�·����ʾ�� lineEditInput ��
    if (!inputFilePath.isEmpty()) {
        ui.lineEditInput->setText(inputFilePath);
    }
}

// ѡ������ļ���
void Buffer::on_pushButtonOutput_clicked() {
    // �����ļ���ѡ��Ի���
    outputFilePath = QFileDialog::getExistingDirectory(this, "select a folder", "");
    // ��ѡ����ļ���·����ʾ�� lineEditOutput ��
    if (!outputFilePath.isEmpty()) {
        ui.lineEditOutput->setText(outputFilePath);
    }
}

// ���л���������
void Buffer::on_pushButtonOK_clicked() {
    // ��ȡ�������ļ����Ͱ뾶
    outputFileName = ui.lineEditFilename->text();
    QString radiusText = ui.lineEditRadius->text();

    // ��������ļ�·��������ļ�·���Ƿ���Ч
    if (inputFilePath.isEmpty()) { // ����Ƿ�ѡ���������ļ�
        QMessageBox::warning(this, "Input Error", "Please select an input SHP file.");
        return;
    }
    if (outputFilePath.isEmpty()) { // ����Ƿ�ѡ�������·��
        QMessageBox::warning(this, "Output Error", "Please select an output directory.");
        return;
    }
    if (outputFileName.isEmpty()) { // ����Ƿ�����������ļ���
        QMessageBox::warning(this, "File Name Error", "Please provide a file name for the output.");
        return;
    }
    if (radiusText.isEmpty()) { // ����Ƿ������˻������뾶
        QMessageBox::warning(this, "Radius Error", "Please provide a buffer radius.");
        return;
    }

    // ת���������뾶Ϊ������
    bool radiusOk;
    bufferRadius = radiusText.toFloat(&radiusOk);
    if (!radiusOk || bufferRadius <= 0) {
        QMessageBox::warning(this, "Radius Error", "Please provide a valid positive buffer radius.");
        return;
    }

    // ʹ�� OGR ��������ʸ���ļ�
    QgsVectorLayer* layer = new QgsVectorLayer(inputFilePath, "Input Layer", "ogr");
    if (!layer->isValid()) {
        QMessageBox::critical(this, "Layer Error", "Failed to load the input SHP file.");
        return;
    }

    // �������������ĺͷ�������
    QgsProcessingContext context;
    QgsProcessingFeedback feedback;

    // �޸�������ͶӰ����
    QVariantMap fixParams, reprojParams, bufferParams; // �������ӳ��
    fixParams.insert("INPUT", inputFilePath);          // ���������ļ�·��
    fixParams.insert("OUTPUT", "memory:");             // ������ڴ棨��ʱͼ�㣩

    // ���ü����޸��㷨
    auto* fixAlg = QgsApplication::processingRegistry()->algorithmById("native:fixgeometries");
    if (!fixAlg) {
        QMessageBox::critical(this, "Error", "Failed to find fix geometries algorithm.");
        return;
    }
    QVariantMap fixResults = fixAlg->run(fixParams, context, &feedback); // ִ�м����޸�

    // ��������ͶӰ����
    reprojParams.insert("INPUT", fixResults.value("OUTPUT"));  // �޸����ͼ����Ϊ����
    reprojParams.insert("TARGET_CRS", "EPSG:32633");           // ����ͶӰ����ϵ
    reprojParams.insert("OUTPUT", "memory:");                 // ������ڴ�

    // ��������ͶӰ�㷨
    auto* reprojAlg = QgsApplication::processingRegistry()->algorithmById("native:reprojectlayer");
    if (!reprojAlg) {
        QMessageBox::critical(this, "Error", "Failed to find reproject layer algorithm.");
        return;
    }
    QVariantMap reprojResults = reprojAlg->run(reprojParams, context, &feedback); // ִ������ͶӰ

    // ���û�������������
    QString outputFilePathWithName = outputFilePath + "/" + ui.lineEditFilename->text() + ".shp"; // ����ļ�·��
    bufferParams.insert("INPUT", reprojResults.value("OUTPUT")); // ʹ������ͶӰ���ͼ����Ϊ����
    bufferParams.insert("DISTANCE", bufferRadius);              // ���û������뾶
    bufferParams.insert("DISSOLVE", true);                      // �����Ƿ�ϲ����������
    bufferParams.insert("OUTPUT", outputFilePathWithName);      // ��������ļ�·��

    // ���û������㷨
    auto* bufferAlg = QgsApplication::processingRegistry()->algorithmById("native:buffer");
    if (!bufferAlg) {
        QMessageBox::critical(this, "Error", "Failed to find buffer algorithm.");
        return;
    }
    bufferAlg->run(bufferParams, context, &feedback); // ִ�л���������

      // ���� QgsVectorLayer  
    QgsVectorLayer* pointLayer = new QgsVectorLayer(outputFilePathWithName, outputFileName, "ogr");
    if (!pointLayer->isValid()) {
        QMessageBox::critical(this, "����", "�޷����� Shapefile ͼ�㡣");
        return;
    }
    //��ת�����shp�ļ���ͼ����
    QgsProject::instance()->addMapLayer(pointLayer);

    // �����ɹ�����ʾ�ɹ���Ϣ
    QMessageBox::information(this, "Success", "Buffer analysis completed successfully.\nOutput file: " + outputFilePathWithName);
}
