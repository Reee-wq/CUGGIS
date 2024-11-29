
//3
#include "readExcel.h"
#include "QFileDialog"
#include <QtXlsx>
#include <QgsVectorLayer.h>
#include <QMessageBox>
#include <QgsProject.h>
#include <QgsGeometry.h>
#include <qgsattributetableview.h>
#include <qgsattributetablemodel.h>
#include <qgsattributetablefiltermodel.h>
#include <qgspointxy.h>
#include <QDateTime>
#include "xlsxdocument.h"
#include "xlsxworksheet.h"
#include "xlsxcell.h"
#include <gdal.h>
#include "ogrsf_frmts.h"
#include "gdal_priv.h"
#include "gdal.h"  
#include "ogrsf_frmts.h"
#include<ogr_api.h >
#include< ogr_core.h>

readExcel::readExcel(QgsMapCanvas* mapCanvas, QWidget* parent)
    : QMainWindow(parent), m_mapCanvas(mapCanvas)
{
    ui.setupUi(this);
    connect(ui.selectPath, &QPushButton::clicked, this, &readExcel::selectPath);
    connect(ui.comboBoxX, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &readExcel::comboBoxX);
    connect(ui.comboBoxY, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &readExcel::comboBoxY);
    connect(ui.outPath, &QPushButton::clicked, this, &readExcel::outPath);
}

readExcel::~readExcel() {}


// ѡ��EXCEL·��
void readExcel::selectPath() 
{
    excelFilePath = QFileDialog::getOpenFileName(this, QStringLiteral("ѡ��Excel�ļ�"), "", "Excel Files (*.xls *.xlsx)");
    ui.lineEdit->setText(excelFilePath);
    loadExcelFields();
}

// ѡ�����Shapefile�ļ�·��
void readExcel::outPath() {
    outPutFilePath= QFileDialog::getSaveFileName( this,QStringLiteral("ѡ�����Shapefile�ļ�"),"","Shapefile (*.shp)");
    ui.lineEdit_2->setText(outPutFilePath);
    QFileInfo fileInfo(outPutFilePath);
    layerName = fileInfo.baseName();  // ��ȡ������չ�����ļ���
}

void readExcel::loadExcelFields()  // ���combox����������
{
    QXlsx::Document excelFile(excelFilePath);

    // ��ȡ��һ�У������У�������
    QStringList headerRow;
    for (int col = 1; col <= excelFile.dimension().columnCount(); ++col)
    {
        headerRow.append(excelFile.read(1, col).toString());
    }

    ui.comboBoxY->addItems(headerRow);
    ui.comboBoxX->addItems(headerRow);
}

void readExcel::comboBoxX(int index)
{
    // ��ȡX�ֶ�����
    QString selectedXField = ui.comboBoxX->itemText(index);
}

void readExcel::comboBoxY(int index)
{
    // ��ȡY�ֶ�����
    QString selectedYField = ui.comboBoxY->itemText(index);
}

void readExcel::on_pushButtonOK_clicked()
{
    // ȷ�� comboBoxX �� comboBoxY �Ѿ�ѡ������Ч���ֶ�
    QString selectedXField = ui.comboBoxX->currentText();
    QString selectedYField = ui.comboBoxY->currentText();

    if (selectedXField.isEmpty() || selectedYField.isEmpty())
    {
        QMessageBox::warning(this, "����", "��ѡ����Ч�� X �� Y �ֶΡ�");
        return;
    }
    // �� Excel �ļ�
    QXlsx::Document excelFile(excelFilePath);
     // ���� GDAL Shapefile ����  
    GDALAllRegister();
    GDALDriverManager* poDriverManager = GetGDALDriverManager();
    GDALDriver* poDriver = poDriverManager->GetDriverByName("ESRI Shapefile");

    if (poDriver == nullptr) {
        QMessageBox::critical(this, "����", "�޷����� Shapefile ������");
        return;
    }

    // ������� Shapefile  
    GDALDataset* poDataset = poDriver->Create(outPutFilePath.toStdString().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
    if (poDataset == nullptr) {
        QMessageBox::critical(this, "����", "�޷����� Shapefile �ļ���");
        return;
    }

    // ����һ����ͼ��  
    OGRLayer* poLayer = poDataset->CreateLayer(layerName.toStdString().c_str(), nullptr, wkbPoint, nullptr);

    // �����ͼ�������  
    for (int col = 1; col <= excelFile.dimension().columnCount(); ++col) {
        QString fieldName = excelFile.read(1, col).toString();
        OGRFieldDefn oField(fieldName.toStdString().c_str(), OFTString);
        poLayer->CreateField(&oField);
    }

    // ���� Excel ���ݲ����Ҫ��  
    for (int row = 2; row <= excelFile.dimension().rowCount(); ++row) {
        QVariant xValue = excelFile.read(row, ui.comboBoxX->currentIndex() + 1);
        QVariant yValue = excelFile.read(row, ui.comboBoxY->currentIndex() + 1);

        if (xValue.isValid() && yValue.isValid()) {
            // ��ȡ X, Y ����  
            double x = xValue.toDouble();
            double y = yValue.toDouble();

            // ������Ҫ��  
            OGRPoint point(x, y);
            OGRFeature* poFeature = new OGRFeature(poLayer->GetLayerDefn());
            poFeature->SetGeometry(&point);

            // ����ֶ�ֵ  
            for (int col = 1; col <= excelFile.dimension().columnCount(); ++col) {
                QString fieldName = excelFile.read(1, col).toString();
                QVariant fieldValue = excelFile.read(row, col);
                poFeature->SetField(fieldName.toStdString().c_str(), fieldValue.toString().toStdString().c_str());
            }

            // ���Ҫ�ص�ͼ��  
            poLayer->CreateFeature(poFeature);
            OGRFeature::DestroyFeature(poFeature);
        }
    }

    // ������Դ  
    GDALClose(poDataset);

    // ���� QgsVectorLayer  
    QgsVectorLayer* pointLayer = new QgsVectorLayer(outPutFilePath, layerName, "ogr");
    if (!pointLayer->isValid()) {
        QMessageBox::critical(this, "����", "�޷����� Shapefile ͼ�㡣");
        return;
    }
    //��ת�����shp�ļ���ͼ����
    QgsProject::instance()->addMapLayer(pointLayer);

    // ��ȡ QgsVectorLayerCache  
    QgsVectorLayerCache* layerCache = new QgsVectorLayerCache(pointLayer, pointLayer->featureCount());

    // ���� Attribute Table Model  
    QgsAttributeTableModel* model = new QgsAttributeTableModel(layerCache);
    model->loadLayer();

    // ��������ģ��  
    QgsAttributeTableFilterModel* filterModel = new QgsAttributeTableFilterModel(m_mapCanvas, model, model);
    filterModel->setFilterMode(QgsAttributeTableFilterModel::ShowAll);

    // ��ʾ���Ա�  
    QgsAttributeTableView* attrTableView = new QgsAttributeTableView();
    attrTableView->setModel(filterModel);
    attrTableView->show();

    // �رյ�ǰ����  
    close();
}

    //// �� Excel �ļ�
    //QXlsx::Document excelFile(excelFilePath);

    //// ����ʸ��ͼ��
    //QgsVectorLayer* pointLayer = new QgsVectorLayer("Point?crs=EPSG:4326", layerName, "memory");
    //if (!pointLayer) {
    //    QMessageBox::critical(this, "����", "�޷�����ʸ��ͼ�㡣");
    //    return;
    //}

    //// ��ʸ��ͼ������ֶ�
    //QStringList headerRow;
    //for (int col = 1; col <= excelFile.dimension().columnCount(); ++col)
    //{
    //    QString fieldName = excelFile.read(1, col).toString();
    //    QVariant::Type dataType = QVariant::Invalid;

    //    // �ж���������
    //    for (int row = 2; row <= excelFile.dimension().rowCount(); ++row)
    //    {
    //        QVariant cellValue = excelFile.read(row, col);
    //        if (cellValue.isValid())
    //        {
    //            dataType = cellValue.type();
    //            break;
    //        }
    //    }

    //    if (dataType != QVariant::Invalid)
    //    {
    //        QgsField field(fieldName, dataType);
    //        pointLayer->dataProvider()->addAttributes({ field });
    //    }
    //}
    //pointLayer->updateFields();

    //// ��ȡѡ���ֶε��к�
    //int xCol = -1;
    //int yCol = -1;

    //for (int col = 1; col <= excelFile.dimension().columnCount(); ++col)
    //{
    //    QString fieldName = excelFile.read(1, col).toString();
    //    if (fieldName == selectedXField)
    //    {
    //        xCol = col;
    //    }
    //    else if (fieldName == selectedYField)
    //    {
    //        yCol = col;
    //    }
    //}

    //if (xCol == -1 || yCol == -1) {
    //    QMessageBox::critical(this, "����", "δ���ҵ�ѡ���ֶε��кš�");
    //    return;
    //}

    //// ��ʸ��ͼ�����Ҫ��
    //QgsFeatureList features;
    //for (int row = 2; row <= excelFile.dimension().rowCount(); ++row)
    //{
    //    double xCoord = excelFile.read(row, xCol).toDouble();  // ���� X��Y ����
    //    double yCoord = excelFile.read(row, yCol).toDouble();

    //    QgsFeature feature;
    //    QgsGeometry geometry = QgsGeometry::fromPointXY(QgsPointXY(xCoord, yCoord));
    //    feature.setGeometry(geometry);

    //    // ��������
    //    for (int col = 1; col <= excelFile.dimension().columnCount(); ++col)
    //    {
    //        QString fieldName = excelFile.read(1, col).toString();
    //        QVariant cellValue = excelFile.read(row, col);

    //        // ���������Ϣ��ȷ���ֶ����ݶ�ȡ��ȷ
    //        qDebug() << "Field: " << fieldName << " Value: " << cellValue;

    //        // �����ֶεĴ���
    //        if (fieldName == "time") {
    //            QDateTime dateTime = QDateTime::fromString(cellValue.toString(), Qt::ISODate);
    //            feature.setAttribute(fieldName, dateTime);
    //        }
    //        // �����ַ�������
    //        else if (cellValue.type() == QVariant::String)
    //        {
    //            feature.setAttribute(fieldName, cellValue.toString());
    //        }
    //        // ����������
    //        else if (cellValue.type() == QVariant::Double)
    //        {
    //            feature.setAttribute(fieldName, cellValue.toDouble());
    //        }
    //        // ������������
    //        else if (cellValue.type() == QVariant::Int)
    //        {
    //            feature.setAttribute(fieldName, cellValue.toInt());
    //        }
    //        else
    //        {
    //            feature.setAttribute(fieldName, cellValue);  // ʹ�� QVariant ��Ӧ��������
    //        }
    //    }

    //    features.append(feature);
    //}
    //pointLayer->dataProvider()->addFeatures(features);

    //// ��ʸ��ͼ����ӵ���Ŀ��
    //QgsProject::instance()->addMapLayer(pointLayer);

    //// ���� QgsVectorLayerCache ����
    //QgsVectorLayerCache* layerCache = new QgsVectorLayerCache(pointLayer, pointLayer->featureCount());

    //// ���� Attribute Table Model
    //QgsAttributeTableModel* model = new QgsAttributeTableModel(layerCache);

    //// ����ͼ������
    //model->loadLayer();

    //// ��������ģ��
    //QgsAttributeTableFilterModel* filterModel = new QgsAttributeTableFilterModel(m_mapCanvas, model, model);
    //filterModel->setFilterMode(QgsAttributeTableFilterModel::ShowAll);

    //// ��ʾ���Ա�
    //QgsAttributeTableView* attrTableView = new QgsAttributeTableView();
    //attrTableView->setModel(filterModel);
    //attrTableView->show();

    //// �رյ�ǰ����
    //close();
//}
