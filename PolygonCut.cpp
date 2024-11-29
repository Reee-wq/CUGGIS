#include "PolygonCut.h"
#include <QgsGeometry.h>
#include <QgsFeature.h>
#include <QgsFeatureIterator.h>
#include <QgsProject.h>
#include <QgsVectorLayer.h>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QKeyEvent>

// 构造函数
PolygonCut::PolygonCut(QgsMapCanvas* Mapcanvas) : QgsMapTool(Mapcanvas)
{
    pLayer = nullptr;
    mCanvas = Mapcanvas;
    rubberBand_ = new QgsRubberBand(mCanvas, QgsWkbTypes::PolygonGeometry);
    rubberBand_->setColor(Qt::red);
    rubberBand_->setWidth(2);
    highlightRubberBand_ = nullptr;
    blinkTimer_ = nullptr;
    StatusFlag = false;
}

// 析构函数
PolygonCut::~PolygonCut()
{
}

// 设置当前图层
void PolygonCut::SetSelectLayer(QgsVectorLayer* Layer)
{
    pLayer = Layer;
}

// 鼠标左键按下 - 添加多边形顶点
void PolygonCut::canvasPressEvent(QgsMapMouseEvent* e)
{
    if (!pLayer || e->button() != Qt::LeftButton)
        return;

    // 记录点到多边形顶点列表
    QgsPointXY point = e->mapPoint();
    polygonVertices_.append(point);
    rubberBand_->addPoint(point);
}

// 鼠标右键 - 闭合多边形并高亮内容
void PolygonCut::canvasReleaseEvent(QgsMapMouseEvent* e)
{
    if (!pLayer || e->button() != Qt::RightButton || polygonVertices_.size() < 3)
        return;

    // 闭合多边形
    polygonVertices_.append(polygonVertices_.first());
    rubberBand_->addPoint(polygonVertices_.first());

    // 创建 QgsGeometry (这里是多边形的选择区域)
    QVector<QgsPointXY> points = QVector<QgsPointXY>::fromList(polygonVertices_);
    QgsPolygonXY polygonXY;
    polygonXY.append(points);
    selectionGeometry = QgsGeometry::fromPolygonXY(polygonXY);  // 存储到类成员中

    // 筛选多边形内的要素
    QgsFeatureIterator iter = pLayer->getFeatures();
    QgsFeature feature;
    selectedFeatures_.clear();

    while (iter.nextFeature(feature)) {
        if (selectionGeometry.intersects(feature.geometry())) {
            selectedFeatures_.insert(feature.id());
        }
    }

    // 高亮选中的要素
    highlightSelectedFeatures();

    // 重置顶点列表和橡皮筋
    polygonVertices_.clear();
    rubberBand_->reset(QgsWkbTypes::PolygonGeometry);
}

// 高亮选中的要素
void PolygonCut::highlightSelectedFeatures()
{
    // 移除之前的高亮显示
    if (highlightRubberBand_ != nullptr) {
        mCanvas->scene()->removeItem(highlightRubberBand_);
        delete highlightRubberBand_;
        highlightRubberBand_ = nullptr;
    }

    if (!pLayer || selectedFeatures_.isEmpty())
        return;

    // 创建新的橡皮筋线用于高亮
    highlightRubberBand_ = new QgsRubberBand(mCanvas, QgsWkbTypes::PolygonGeometry);
    highlightRubberBand_->setColor(Qt::blue);
    highlightRubberBand_->setWidth(2);

    QgsFeatureRequest request;
    request.setFilterFids(selectedFeatures_);
    QgsFeatureIterator iter = pLayer->getFeatures(request);
    QgsFeature feature;

    // 遍历选中的要素并计算交集
    while (iter.nextFeature(feature)) {
        QgsGeometry featureGeometry = feature.geometry();

        // 计算交集部分
        QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);

        // 如果交集部分不为空，将其添加到橡皮筋线中
        if (!intersectionGeometry.isEmpty()) {
            highlightRubberBand_->addGeometry(intersectionGeometry);
        }
    }

    // 设置高亮显示的闪烁效果
    if (blinkTimer_ != nullptr) {
        blinkTimer_->stop();
        delete blinkTimer_;
        blinkTimer_ = nullptr;
    }

    blinkTimer_ = new QTimer(this);
    connect(blinkTimer_, &QTimer::timeout, this, [this]() {
        if (highlightRubberBand_->isVisible()) {
            highlightRubberBand_->hide();
        }
        else {
            highlightRubberBand_->show();
        }
        });

    blinkTimer_->start(500); // 设置闪烁时间间隔为500ms
}

// 键盘事件 - Delete键保存选中要素
void PolygonCut::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Delete) {
        saveHighlightedFeaturesToShapefile();
    }
}

// 保存高亮显示的要素到 Shapefile
void PolygonCut::saveHighlightedFeaturesToShapefile()
{
    if (!pLayer || selectedFeatures_.isEmpty()) {
        QMessageBox::warning(nullptr, "警告", "未选择任何要素！");
        return;
    }

    // 停止闪烁效果
    if (blinkTimer_ != nullptr) {
        blinkTimer_->stop();
        delete blinkTimer_;
        blinkTimer_ = nullptr;
    }

    // 弹出文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(nullptr, "保存裁剪后的文件", "", "Shapefile (*.shp)");

    if (filePath.isEmpty())
        return;

    // 注册所有格式驱动
    GDALAllRegister();

    // 获取 Shapefile 驱动
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if (!poDriver) {
        QMessageBox::critical(nullptr, "错误", "Shapefile 驱动不可用。");
        return;
    }

    // 创建新的数据源
    GDALDataset* poDS = poDriver->Create(filePath.toStdString().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
    if (!poDS) {
        QMessageBox::critical(nullptr, "错误", "创建 Shapefile 失败。");
        return;
    }

    // 确定几何类型
    QgsWkbTypes::GeometryType geomType = pLayer->geometryType();
    OGRwkbGeometryType ogrType = (geomType == QgsWkbTypes::PointGeometry) ? wkbPoint :
        (geomType == QgsWkbTypes::LineGeometry) ? wkbLineString :
        wkbPolygon;

    // 创建新的图层
    OGRLayer* poLayer = poDS->CreateLayer("Layer", nullptr, ogrType, nullptr);
    if (!poLayer) {
        QMessageBox::critical(nullptr, "错误", "创建图层失败。");
        GDALClose(poDS);
        return;
    }

    // 遍历交集几何体并写入 Shapefile
    QgsFeatureRequest request;
    request.setFilterFids(selectedFeatures_);
    QgsFeatureIterator iter = pLayer->getFeatures(request);
    QgsFeature feature;

    while (iter.nextFeature(feature)) {
        QgsGeometry featureGeometry = feature.geometry();
        QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);

        if (!intersectionGeometry.isEmpty()) {
            // 创建 OGRFeature
            OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

            // 转换几何体为 WKT 并创建 OGR 几何体
            OGRGeometry* poGeometry = nullptr;
            std::string wkt = intersectionGeometry.asWkt().toStdString();
            OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &poGeometry);

            if (!poGeometry) {
                QMessageBox::warning(nullptr, "警告", "几何体转换失败，跳过当前要素。");
                OGRFeature::DestroyFeature(poFeature);
                continue;
            }

            poFeature->SetGeometry(poGeometry);

            // 写入要素到图层
            if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
                QMessageBox::warning(nullptr, "警告", "写入要素失败。");
            }

            // 清理内存
            OGRFeature::DestroyFeature(poFeature);
            OGRGeometryFactory::destroyGeometry(poGeometry);
        }
    }

    // 清理数据源
    GDALClose(poDS);

    // 获取文件名并加载到 QGIS
    QFileInfo fileInfo(filePath);
    QString outputFileName = fileInfo.baseName();

    QgsVectorLayer* newLayer = new QgsVectorLayer(filePath, outputFileName, "ogr");
    if (!newLayer->isValid()) {
        QMessageBox::warning(nullptr, "错误", "无法加载 Shapefile 图层。");
        return;
    }

    // 将新图层添加到 QGIS 项目
    QgsProject::instance()->addMapLayer(newLayer);

    // 提示保存成功
    QMessageBox::information(nullptr, "保存成功", "Shapefile 已成功保存并加载到地图中。");
}

// 设置启用状态
void PolygonCut::SetEnable(bool flag)
{
    StatusFlag = flag;
    if (StatusFlag) {
        mCursor = Qt::CrossCursor;
    }
    else {
        mCursor = Qt::ArrowCursor;
    }
}
