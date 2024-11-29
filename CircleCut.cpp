/**************************圆形裁剪工具*********************
*****************************************************************/

#include "CircleCut.h"
#include "qgsgeometry.h"
#include <QgsProject.h>
#include <QMessageBox>

//构造和析构函数
CircleCut::CircleCut(QgsMapCanvas* Mapcanvas) :QgsMapTool(Mapcanvas)
{
    pLayer = NULL;
    mCursor = Qt::ArrowCursor;
    mCanvas = Mapcanvas;
    StatusFlag = true;
    rubberBand_ = new  QgsRubberBand(mCanvas);
}

CircleCut::~CircleCut(void)
{
}

// 设置当前被选择(活动)的图层
void CircleCut::SetSelectLayer(QgsVectorLayer* Layer)
{
    pLayer = Layer;
}

// 拉框鼠标按下
void CircleCut::canvasPressEvent(QgsMapMouseEvent* e)
{
    if (mCanvas == nullptr || pLayer == nullptr) return;
    if (e->button() == Qt::LeftButton) {
        startPoint_ = e->mapPoint(); //存储选择框的起始点
        rubberBand_->setColor(Qt::red); //橡皮筋颜色
        rubberBand_->setWidth(2); //线宽
        rubberBand_->addPoint(startPoint_);
        selecting_ = true;
    }
}

// 鼠标移动
void CircleCut::canvasMoveEvent(QgsMapMouseEvent* e)
{
    if (mCanvas == nullptr || pLayer == nullptr || !selecting_) return;
    endPoint_ = e->mapPoint(); // 更新结束点

    // 更新橡皮筋，绘制圆形选择区域
    rubberBand_->reset();

    // 传递 startPoint_ 和 endPoint_ 的 x 和 y 值给 SetRubberBand
    SetRubberBand(startPoint_.x(), startPoint_.y(), endPoint_.x(), endPoint_.y(), rubberBand_);
}


// 拉框鼠标释放
void CircleCut::canvasReleaseEvent(QgsMapMouseEvent* e) {
    if (mCanvas == nullptr || pLayer == nullptr || !selecting_) return;
    if (e->button() == Qt::LeftButton) {
        selecting_ = false;
        endPoint_ = e->mapPoint();

        // 使用 fromPointXY 创建几何体
        double radius = startPoint_.distance(endPoint_);
        QgsGeometry selectionGeometry = QgsGeometry::fromPointXY(QgsPointXY(startPoint_)).buffer(radius, 0);

        // 使用 setFilterRect 代替 setFilterGeometry
        QgsFeatureRequest request;
        request.setFilterRect(selectionGeometry.boundingBox());  // 使用包围盒进行过滤

        QgsFeatureIterator iter = pLayer->getFeatures(request);
        QgsFeature feature;
        bool doDifference = (e->modifiers() & Qt::ControlModifier);
        while (iter.nextFeature(feature)) {
            QgsGeometry featureGeometry = feature.geometry();
            if (selectionGeometry.intersects(featureGeometry)) {
                QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);
                if (!intersectionGeometry.isEmpty()) {
                    if (doDifference) {
                        selectedFeatures_.remove(feature.id());
                    }
                    else {
                        selectedFeatures_.insert(feature.id());
                    }
                }
            }
        }

        highlightSelectedFeatures();
        if (rubberBand_ != nullptr) {
            rubberBand_->reset();
        }
    }
}

// 高亮显示
void CircleCut::highlightSelectedFeatures() {
    // 移除之前的高亮显示（如果有的话）
    if (highlightRubberBand_ != nullptr) {
        mCanvas->scene()->removeItem(highlightRubberBand_);
        delete highlightRubberBand_;
        highlightRubberBand_ = nullptr;
    }

    if (!selectedFeatures_.isEmpty()) {
        // 创建一个新的橡皮筋线来高亮显示选中的部分
        highlightRubberBand_ = new QgsRubberBand(mCanvas, pLayer->geometryType());
        highlightRubberBand_->setColor(Qt::blue);  // 设置高亮颜色
        highlightRubberBand_->setWidth(2);  // 设置边框宽度

        // 遍历所有被选中的要素ID，获取它们与选择框交集部分的几何体
        QgsFeatureRequest request;
        request.setFilterFids(selectedFeatures_);
        QgsFeatureIterator iter = pLayer->getFeatures(request);
        QgsFeature feature;
        while (iter.nextFeature(feature)) {
            QgsGeometry featureGeometry = feature.geometry();  // 获取要素几何体
            QgsGeometry selectionGeometry = QgsGeometry::fromPointXY(QgsPointXY(startPoint_)).buffer(startPoint_.distance(endPoint_), 20);  // 创建圆形缓冲区
            // 计算选择框与要素的交集部分
            QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);

            // 如果交集部分不为空，则将其添加到橡皮筋线中
            if (!intersectionGeometry.isEmpty()) {
                highlightRubberBand_->addGeometry(intersectionGeometry);
            }
        }

        // 设置高亮显示的闪烁效果（可选）
        if (blinkTimer_ != nullptr) {
            blinkTimer_->stop();  // 停止定时器
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

        blinkTimer_->start(500);  // 设置闪烁时间间隔为500ms
    }
}


// 绘制圆形选择框
void CircleCut::SetRubberBand(double startX, double startY, double endX, double endY, QgsRubberBand* rubberBand) {
    // 创建和设置橡皮筋
    QgsPointXY startPoint(startX, startY);
    QgsPointXY endPoint(endX, endY);

    // 在这里你可以根据需要绘制圆形、线段等
    rubberBand->addPoint(startPoint);
    rubberBand->addPoint(endPoint);  // 示例，根据实际需求修改
}

void CircleCut::SetEnable(bool flag)
{
    StatusFlag = flag;
    if (StatusFlag) {
        mCursor = Qt::CrossCursor;
    }
    else {
        mCursor = Qt::ArrowCursor;
    }
}

// 监听键盘事件
void CircleCut::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Delete) {
        // 处理删除操作
        deleteSelectedFeatures();
    }
}

// 删除并导出选中的要素
void CircleCut::deleteSelectedFeatures()
{
    if (mCanvas == nullptr || pLayer == nullptr || selectedFeatures_.isEmpty()) {
        return;
    }

    // 创建一个圆形选择框几何体，使用 QgsPointXY 和 buffer 来创建圆形
    double radius = startPoint_.distance(endPoint_); // 半径为起始点和结束点之间的距离
    QgsPointXY point(startPoint_); // 创建点几何
    QgsGeometry highlightGeometry = QgsGeometry::fromPointXY(point).buffer(radius, 20); // 20 是缓冲区的分段数，可根据需求调整

    // 创建一个新的列表，用于存储裁剪后的几何体
    QList<QgsGeometry> geometriesToSave;

    // 获取当前图层的几何类型
    QgsWkbTypes::GeometryType geomType = pLayer->geometryType();

    // 遍历选中的所有要素
    QgsFeatureIterator iter = pLayer->getFeatures(QgsFeatureRequest().setFilterFids(selectedFeatures_));
    QgsFeature feature;
    while (iter.nextFeature(feature)) {
        QgsGeometry featureGeometry = feature.geometry();

        // 判断几何类型并进行相应的裁剪
        if (geomType == QgsWkbTypes::PointGeometry) {
            // 点：检查点是否在选择框内
            if (highlightGeometry.contains(featureGeometry)) {
                geometriesToSave.append(featureGeometry); // 点直接添加到保存列表中
            }
        }
        else if (geomType == QgsWkbTypes::LineGeometry) {
            // 线：计算线与选择框的交集
            QgsGeometry intersectionGeometry = featureGeometry.intersection(highlightGeometry);
            if (!intersectionGeometry.isEmpty()) {
                geometriesToSave.append(intersectionGeometry); // 保存交集部分
            }
        }
        else if (geomType == QgsWkbTypes::PolygonGeometry) {
            // 面：计算面与选择框的交集
            QgsGeometry intersectionGeometry = featureGeometry.intersection(highlightGeometry);
            if (!intersectionGeometry.isEmpty()) {
                geometriesToSave.append(intersectionGeometry); // 保存交集部分
            }
        }
    }

    // 如果有几何体被裁剪，保存为新的 Shapefile
    if (!geometriesToSave.isEmpty()) {
        if (highlightRubberBand_) {
            highlightRubberBand_->setVisible(false);  // 确保高亮不可见
            mCanvas->scene()->removeItem(highlightRubberBand_);
            delete highlightRubberBand_;
            highlightRubberBand_ = nullptr;
        }

        if (blinkTimer_) {
            blinkTimer_->stop();  // 停止定时器
            delete blinkTimer_;
            blinkTimer_ = nullptr;  // 避免悬空指针
        }

        // 弹出文件保存对话框，让用户选择保存路径
        QString filePath = QFileDialog::getSaveFileName(nullptr, "保存裁剪后的文件", "", "Shapefile (*.shp)");

        if (!filePath.isEmpty()) {
            // 使用 GDAL 创建新的 Shapefile 数据源
            GDALAllRegister(); // 注册所有格式驱动
            GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
            if (poDriver == nullptr) {
                QMessageBox::critical(nullptr, "Error", "Shapefile driver not available.");
                return;
            }

            // 创建新的数据源
            GDALDataset* poDS = poDriver->Create(filePath.toStdString().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
            if (poDS == nullptr) {
                QMessageBox::critical(nullptr, "Error", "Failed to create shapefile.");
                return;
            }

            // 创建新的图层
            OGRwkbGeometryType ogrType = (geomType == QgsWkbTypes::PointGeometry) ? wkbPoint :
                (geomType == QgsWkbTypes::LineGeometry) ? wkbLineString : wkbPolygon;
            OGRLayer* poLayer = poDS->CreateLayer("Layer", nullptr, ogrType, nullptr);
            if (poLayer == nullptr) {
                QMessageBox::critical(nullptr, "Error", "Failed to create layer.");
                GDALClose(poDS);
                return;
            }

            // 写入几何体
            for (const QgsGeometry& geometry : geometriesToSave) {
                OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

                // 设置几何体
                OGRGeometry* poGeometry = nullptr;
                std::string wkt = geometry.asWkt().toStdString();
                OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &poGeometry);
                if (poGeometry == nullptr) {
                    QMessageBox::warning(nullptr, "Warning", "Failed to convert geometry.");
                    continue;
                }
                poFeature->SetGeometry(poGeometry);

                // 写入要素到图层
                if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
                    QMessageBox::warning(nullptr, "Warning", "Failed to write feature.");
                }

                // 清理几何体和要素
                OGRFeature::DestroyFeature(poFeature);
                OGRGeometryFactory::destroyGeometry(poGeometry);
            }

            // 清理数据集
            GDALClose(poDS);

            //获取文件名
            QFileInfo fileInfo(filePath);
            QString outputFileName = fileInfo.baseName();

            // 创建 QgsVectorLayer  
            QgsVectorLayer* pointLayer = new QgsVectorLayer(filePath, outputFileName, "ogr");
            if (!pointLayer->isValid()) {
                QMessageBox::warning(nullptr, "错误", "无法加载 Shapefile 图层。");
                return;
            }
            //将转换后的shp文件在图层上
            QgsProject::instance()->addMapLayer(pointLayer);

            // 提示保存成功
            QMessageBox::information(nullptr, "Export Successful", "Shapefile has been saved successfully.");
        }
    }
}

