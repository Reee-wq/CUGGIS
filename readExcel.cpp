
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


// 选择EXCEL路径
void readExcel::selectPath() 
{
    excelFilePath = QFileDialog::getOpenFileName(this, QStringLiteral("选择Excel文件"), "", "Excel Files (*.xls *.xlsx)");
    ui.lineEdit->setText(excelFilePath);
    loadExcelFields();
}

// 选择输出Shapefile文件路径
void readExcel::outPath() {
    outPutFilePath= QFileDialog::getSaveFileName( this,QStringLiteral("选择输出Shapefile文件"),"","Shapefile (*.shp)");
    ui.lineEdit_2->setText(outPutFilePath);
    QFileInfo fileInfo(outPutFilePath);
    layerName = fileInfo.baseName();  // 获取不带扩展名的文件名
}

void readExcel::loadExcelFields()  // 添加combox下拉框内容
{
    QXlsx::Document excelFile(excelFilePath);

    // 获取第一行（标题行）的数据
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
    // 获取X字段名称
    QString selectedXField = ui.comboBoxX->itemText(index);
}

void readExcel::comboBoxY(int index)
{
    // 获取Y字段名称
    QString selectedYField = ui.comboBoxY->itemText(index);
}

void readExcel::on_pushButtonOK_clicked()
{
    // 确定 comboBoxX 和 comboBoxY 已经选择了有效的字段
    QString selectedXField = ui.comboBoxX->currentText();
    QString selectedYField = ui.comboBoxY->currentText();

    if (selectedXField.isEmpty() || selectedYField.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请选择有效的 X 和 Y 字段。");
        return;
    }
    // 打开 Excel 文件
    QXlsx::Document excelFile(excelFilePath);
     // 创建 GDAL Shapefile 驱动  
    GDALAllRegister();
    GDALDriverManager* poDriverManager = GetGDALDriverManager();
    GDALDriver* poDriver = poDriverManager->GetDriverByName("ESRI Shapefile");

    if (poDriver == nullptr) {
        QMessageBox::critical(this, "错误", "无法加载 Shapefile 驱动。");
        return;
    }

    // 创建输出 Shapefile  
    GDALDataset* poDataset = poDriver->Create(outPutFilePath.toStdString().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
    if (poDataset == nullptr) {
        QMessageBox::critical(this, "错误", "无法创建 Shapefile 文件。");
        return;
    }

    // 创建一个点图层  
    OGRLayer* poLayer = poDataset->CreateLayer(layerName.toStdString().c_str(), nullptr, wkbPoint, nullptr);

    // 定义点图层的属性  
    for (int col = 1; col <= excelFile.dimension().columnCount(); ++col) {
        QString fieldName = excelFile.read(1, col).toString();
        OGRFieldDefn oField(fieldName.toStdString().c_str(), OFTString);
        poLayer->CreateField(&oField);
    }

    // 遍历 Excel 数据并添加要素  
    for (int row = 2; row <= excelFile.dimension().rowCount(); ++row) {
        QVariant xValue = excelFile.read(row, ui.comboBoxX->currentIndex() + 1);
        QVariant yValue = excelFile.read(row, ui.comboBoxY->currentIndex() + 1);

        if (xValue.isValid() && yValue.isValid()) {
            // 获取 X, Y 坐标  
            double x = xValue.toDouble();
            double y = yValue.toDouble();

            // 创建点要素  
            OGRPoint point(x, y);
            OGRFeature* poFeature = new OGRFeature(poLayer->GetLayerDefn());
            poFeature->SetGeometry(&point);

            // 填充字段值  
            for (int col = 1; col <= excelFile.dimension().columnCount(); ++col) {
                QString fieldName = excelFile.read(1, col).toString();
                QVariant fieldValue = excelFile.read(row, col);
                poFeature->SetField(fieldName.toStdString().c_str(), fieldValue.toString().toStdString().c_str());
            }

            // 添加要素到图层  
            poLayer->CreateFeature(poFeature);
            OGRFeature::DestroyFeature(poFeature);
        }
    }

    // 清理资源  
    GDALClose(poDataset);

    // 创建 QgsVectorLayer  
    QgsVectorLayer* pointLayer = new QgsVectorLayer(outPutFilePath, layerName, "ogr");
    if (!pointLayer->isValid()) {
        QMessageBox::critical(this, "错误", "无法加载 Shapefile 图层。");
        return;
    }
    //将转换后的shp文件在图层上
    QgsProject::instance()->addMapLayer(pointLayer);

    // 获取 QgsVectorLayerCache  
    QgsVectorLayerCache* layerCache = new QgsVectorLayerCache(pointLayer, pointLayer->featureCount());

    // 创建 Attribute Table Model  
    QgsAttributeTableModel* model = new QgsAttributeTableModel(layerCache);
    model->loadLayer();

    // 创建过滤模型  
    QgsAttributeTableFilterModel* filterModel = new QgsAttributeTableFilterModel(m_mapCanvas, model, model);
    filterModel->setFilterMode(QgsAttributeTableFilterModel::ShowAll);

    // 显示属性表  
    QgsAttributeTableView* attrTableView = new QgsAttributeTableView();
    attrTableView->setModel(filterModel);
    attrTableView->show();

    // 关闭当前窗口  
    close();
}

    //// 打开 Excel 文件
    //QXlsx::Document excelFile(excelFilePath);

    //// 创建矢量图层
    //QgsVectorLayer* pointLayer = new QgsVectorLayer("Point?crs=EPSG:4326", layerName, "memory");
    //if (!pointLayer) {
    //    QMessageBox::critical(this, "错误", "无法创建矢量图层。");
    //    return;
    //}

    //// 向矢量图层添加字段
    //QStringList headerRow;
    //for (int col = 1; col <= excelFile.dimension().columnCount(); ++col)
    //{
    //    QString fieldName = excelFile.read(1, col).toString();
    //    QVariant::Type dataType = QVariant::Invalid;

    //    // 判断数据类型
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

    //// 获取选定字段的列号
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
    //    QMessageBox::critical(this, "错误", "未能找到选定字段的列号。");
    //    return;
    //}

    //// 向矢量图层添加要素
    //QgsFeatureList features;
    //for (int row = 2; row <= excelFile.dimension().rowCount(); ++row)
    //{
    //    double xCoord = excelFile.read(row, xCol).toDouble();  // 读入 X、Y 坐标
    //    double yCoord = excelFile.read(row, yCol).toDouble();

    //    QgsFeature feature;
    //    QgsGeometry geometry = QgsGeometry::fromPointXY(QgsPointXY(xCoord, yCoord));
    //    feature.setGeometry(geometry);

    //    // 设置属性
    //    for (int col = 1; col <= excelFile.dimension().columnCount(); ++col)
    //    {
    //        QString fieldName = excelFile.read(1, col).toString();
    //        QVariant cellValue = excelFile.read(row, col);

    //        // 输出调试信息，确保字段数据读取正确
    //        qDebug() << "Field: " << fieldName << " Value: " << cellValue;

    //        // 日期字段的处理
    //        if (fieldName == "time") {
    //            QDateTime dateTime = QDateTime::fromString(cellValue.toString(), Qt::ISODate);
    //            feature.setAttribute(fieldName, dateTime);
    //        }
    //        // 处理字符串类型
    //        else if (cellValue.type() == QVariant::String)
    //        {
    //            feature.setAttribute(fieldName, cellValue.toString());
    //        }
    //        // 处理浮动类型
    //        else if (cellValue.type() == QVariant::Double)
    //        {
    //            feature.setAttribute(fieldName, cellValue.toDouble());
    //        }
    //        // 处理整数类型
    //        else if (cellValue.type() == QVariant::Int)
    //        {
    //            feature.setAttribute(fieldName, cellValue.toInt());
    //        }
    //        else
    //        {
    //            feature.setAttribute(fieldName, cellValue);  // 使用 QVariant 适应其它类型
    //        }
    //    }

    //    features.append(feature);
    //}
    //pointLayer->dataProvider()->addFeatures(features);

    //// 将矢量图层添加到项目中
    //QgsProject::instance()->addMapLayer(pointLayer);

    //// 创建 QgsVectorLayerCache 缓存
    //QgsVectorLayerCache* layerCache = new QgsVectorLayerCache(pointLayer, pointLayer->featureCount());

    //// 创建 Attribute Table Model
    //QgsAttributeTableModel* model = new QgsAttributeTableModel(layerCache);

    //// 加载图层数据
    //model->loadLayer();

    //// 创建过滤模型
    //QgsAttributeTableFilterModel* filterModel = new QgsAttributeTableFilterModel(m_mapCanvas, model, model);
    //filterModel->setFilterMode(QgsAttributeTableFilterModel::ShowAll);

    //// 显示属性表
    //QgsAttributeTableView* attrTableView = new QgsAttributeTableView();
    //attrTableView->setModel(filterModel);
    //attrTableView->show();

    //// 关闭当前窗口
    //close();
//}
