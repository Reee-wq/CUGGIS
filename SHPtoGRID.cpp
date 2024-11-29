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

//ѡ��shp�ļ�
void SHPtoGRID::on_pushButtonInput_clicked() {
    // �����ļ�ѡ��Ի���
    inputFilePath = QFileDialog::getOpenFileName(this, "select a SHP file", "", "Shapefiles (*.shp)");
    // ��ѡ����ļ�·����ʾ�� lineEditInput ��
    if (!inputFilePath.isEmpty()) {
        ui.lineEditInput->setText(inputFilePath);
    }
}

// ѡ������ļ���
void SHPtoGRID::on_pushButtonOutput_clicked() {
    // �����ļ���ѡ��Ի���
    outputFilePath = QFileDialog::getExistingDirectory(this, "select a folder", "");
    // ��ѡ����ļ���·����ʾ�� lineEditOutput ��
    if (!outputFilePath.isEmpty()) {
        ui.lineEditOutput->setText(outputFilePath);
    }
}

void SHPtoGRID::on_pushButtonOK_clicked() {
    outputFileName = ui.lineEditFilename->text(); // ��ȡ����ļ���

    // ��������ļ�·��������ļ�·���Ƿ���Ч
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

    QString rasterOutputPath = outputFilePath + "/" + outputFileName + ".tif"; // ���դ��·��

    // ������Ŀ¼�Ƿ��д
    QFileInfo outputDirInfo(outputFilePath);
    if (!outputDirInfo.isWritable()) {
        QMessageBox::critical(this, "Permission Error", "Output directory is not writable.");
        return;
    }

    // ����ļ��Ƿ��Ѵ��ڣ�����Ѵ��ڣ�ɾ����
    QFileInfo rasterFileInfo(rasterOutputPath);
    if (rasterFileInfo.exists()) {
        QFile::remove(rasterOutputPath); // ɾ�������ļ�
    }

    // ��ʼ�� GDAL
    GDALAllRegister();
    GDALDataset* poDataset = static_cast<GDALDataset*>(GDALOpenEx(inputFilePath.toStdString().c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
    if (!poDataset) {
        QMessageBox::critical(this, "GDAL Error", "Failed to open input SHP file.");
        return;
    }

    // ��ȡʸ��ͼ��
    OGRLayer* poLayer = poDataset->GetLayer(0);
    if (!poLayer) {
        QMessageBox::critical(this, "Layer Error", "Failed to get layer from SHP file.");
        GDALClose(poDataset);
        return;
    }

    // ��ȡʸ���ķ�Χ
    OGREnvelope envelope;
    if (poLayer->GetExtent(&envelope) != OGRERR_NONE) {
        QMessageBox::critical(this, "Extent Error", "Failed to get spatial extent from the input SHP file.");
        GDALClose(poDataset);
        return;
    }

    // ��鷶Χ�Ƿ����
    if (envelope.MinX >= envelope.MaxX || envelope.MinY >= envelope.MaxY) {
        QMessageBox::critical(this, "Extent Error", "Invalid spatial extent. Check the input SHP file.");
        GDALClose(poDataset);
        return;
    }

    // ��̬�������ش�С
    double maxResolution = 5000.0; // ���ֱ�������
    double width = envelope.MaxX - envelope.MinX;
    double height = envelope.MaxY - envelope.MinY;

    double pixelSize = std::max(width, height) / maxResolution;
    qDebug() << "Dynamic Pixel Size: " << pixelSize;

    // ����դ���Ⱥ͸߶�
    int rasterWidth = static_cast<int>(std::ceil(width / pixelSize));
    int rasterHeight = static_cast<int>(std::ceil(height / pixelSize));

    qDebug() << "Raster Dimensions: " << rasterWidth << "x" << rasterHeight;

    // ���դ��ߴ��Ƿ����
    if (rasterWidth <= 0 || rasterHeight <= 0 || rasterWidth > 10000 || rasterHeight > 10000) {
        QMessageBox::critical(this, "Raster Size Error", "Raster dimensions are invalid or too large. Check pixel size or input extent.");
        GDALClose(poDataset);
        return;
    }

    // ȷ�� GDAL ֧�� GTiff ����
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (!poDriver) {
        QMessageBox::critical(this, "Driver Error", "GTiff driver not initialized.");
        GDALClose(poDataset);
        return;
    }

    // �������դ��
    char** papszOptions = nullptr;
    GDALDataset* poRasterDS = poDriver->Create(rasterOutputPath.toUtf8().constData(), rasterWidth, rasterHeight, 1, GDT_Byte, papszOptions);
    if (!poRasterDS) {
        QMessageBox::critical(this, "Output Error", "Failed to create output raster file.");
        GDALClose(poDataset);
        return;
    }

    // ���õ���任����
    double adfGeoTransform[6] = { envelope.MinX, pixelSize, 0, envelope.MaxY, 0, -pixelSize };
    poRasterDS->SetGeoTransform(adfGeoTransform);

    // ��������ο�ϵ
    char* pszWKT = nullptr;
    poLayer->GetSpatialRef()->exportToWkt(&pszWKT);
    poRasterDS->SetProjection(pszWKT);
    CPLFree(pszWKT);

    // ���岢��ʼ�� pahLayers
    OGRLayerH pahLayers[] = { static_cast<OGRLayerH>(poLayer) }; // �� OGRLayer ת��Ϊ OGRLayerH ����
    int bandList[1] = { 1 }; // �����б�������һ������


    // դ��ʸ��ͼ�㣬�ر��������
    if (poLayer->GetGeomType() == wkbPoint || poLayer->GetGeomType() == wkbMultiPoint) {
        // �����ݵ����⴦��
        double burnValue = 1.0;  // ���ֵ
        double pointRadius = pixelSize * 900;//�뾶������Ϊ���ش�С������

        char** rasterizeOptions = nullptr;
        rasterizeOptions = CSLAddString(rasterizeOptions, "-burn");
        rasterizeOptions = CSLAddString(rasterizeOptions, std::to_string(burnValue).c_str());
        rasterizeOptions = CSLAddString(rasterizeOptions, "-at");

        // դ�񻯵�ͼ��
        CPLErr eErr = GDALRasterizeLayers(
            poRasterDS,                   // ���դ�����ݼ�
            1,                            // ��������
            bandList,                     // �����б�
            1,                            // ͼ������
            pahLayers,                    // ʸ��ͼ������
            nullptr,                      // �任����
            nullptr,                      // �任����
            &burnValue,                   // ���ֵ����
            rasterizeOptions,             // դ��ѡ��
            GDALDummyProgress,            // ���Ⱥ���
            nullptr                       // ���Ȳ���
        );

        // �ͷ�ѡ���ڴ�
        CSLDestroy(rasterizeOptions);

        if (eErr != CE_None) {
            QMessageBox::critical(this, "Rasterize Error", "Failed to rasterize point vector data.");
            GDALClose(poRasterDS);
            GDALClose(poDataset);
            return;
        }
    }
    else {
        // ��������ݵı�׼����
        double burnValue = 1.0;           // ���ֵ

        CPLErr eErr = GDALRasterizeLayers(
            poRasterDS,                   // ���դ�����ݼ�
            1,                            // ��������
            bandList,                     // �����б�
            1,                            // ͼ������
            pahLayers,                    // ʸ��ͼ������
            nullptr,                      // �任����
            nullptr,                      // �任����
            &burnValue,                   // ���ֵ����
            nullptr,                      // դ��ѡ��
            GDALDummyProgress,            // ���Ⱥ���
            nullptr                       // ���Ȳ���
        );

        if (eErr != CE_None) {
            QMessageBox::critical(this, "Rasterize Error", "Failed to rasterize vector data.");
            GDALClose(poRasterDS);
            GDALClose(poDataset);
            return;
        }
    }
    // �ر����ݼ�
    GDALClose(poRasterDS);
    GDALClose(poDataset);

    //emit rasterGenerated(rasterOutputPath); // դ����ɺ����ź�
    // ���� QgsRasterLayer
    QgsRasterLayer* rasterLayer = new QgsRasterLayer(rasterOutputPath, outputFileName);
    if (!rasterLayer->isValid()) {
        QMessageBox::critical(this, "����", "cannot load raster");
        return;
    }
    // �����ص�դ��ͼ����ӵ���Ŀ��
    QgsProject::instance()->addMapLayer(rasterLayer);


    QMessageBox::information(this, "Success", "Vector data rasterized successfully.\nOutput file: " + rasterOutputPath);
}
