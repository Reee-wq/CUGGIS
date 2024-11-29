/**************************选择图层要素工具*********************
*****************************************************************/
#include "qgsmaptoolselect.h"
#include "qgsgeometry.h"
#include <QgsProject.h>
#include <QMessageBox>

//构造和析构函数
QgsMapToolSelect::QgsMapToolSelect(QgsMapCanvas* Mapcanvas) :QgsMapTool(Mapcanvas)
{
	pLayer = NULL;
	mCursor = Qt::ArrowCursor;
	mCanvas = Mapcanvas;
	StatusFlag = true;
	rubberBand_ = new  QgsRubberBand(mCanvas);

}
QgsMapToolSelect::~QgsMapToolSelect(void)
{

}

// 设置当前被选择(活动)的图层
void QgsMapToolSelect::SetSelectLayer(QgsVectorLayer* Layer)
{
	pLayer = Layer;
}


// 拉框鼠标按下
void QgsMapToolSelect::canvasPressEvent(QgsMapMouseEvent* e)
{
	if (mCanvas == nullptr || pLayer == nullptr) return;
	if (e->button() == Qt::LeftButton) {
		startPoint_ = e->mapPoint(); //存储选择框的起始点
		//rubberBand_(mCanvas, pLayer->geometryType());
		rubberBand_->setColor(Qt::red); //橡皮筋颜色
		rubberBand_->setWidth(2); //线宽
		rubberBand_->addPoint(startPoint_);
		selecting_ = true;
		//delete rubberBand_;
		//rubberBand_ = nullptr;
	}
}

// 鼠标移动
void QgsMapToolSelect::canvasMoveEvent(QgsMapMouseEvent* e)
{
	if (mCanvas == nullptr || pLayer == nullptr || !selecting_) return;
	endPoint_ = e->mapPoint(); //Update the end point for rectangle

	//Update rubberband to show rectangle
	rubberBand_->reset();
	rubberBand_->addPoint(startPoint_);
	rubberBand_->addPoint(QgsPointXY(endPoint_.x(), startPoint_.y()));
	rubberBand_->addPoint(endPoint_);
	rubberBand_->addPoint(QgsPointXY(startPoint_.x(), endPoint_.y()));
	rubberBand_->closePoints();
}

// 拉框鼠标释放
void QgsMapToolSelect::canvasReleaseEvent(QgsMapMouseEvent* e) {
	if (mCanvas == nullptr || pLayer == nullptr || !selecting_) return;
	if (e->button() == Qt::LeftButton) {
		selecting_ = false;  // 停止选择
		endPoint_ = e->mapPoint();  // 获取鼠标松开时的结束点

		// 创建选择框的矩形几何体
		QgsRectangle rect(startPoint_, endPoint_);
		QgsGeometry selectionGeometry = QgsGeometry::fromRect(rect);  // 将矩形转换为几何对象

		// 设置查询条件，只获取与矩形区域相交的要素
		QgsFeatureRequest request;
		request.setFilterRect(rect);  // 使用矩形区域作为过滤条件，提高效率

		QgsFeatureIterator iter = pLayer->getFeatures(request);  // 获取所有与选择框相交的要素
		QgsFeature feature;
		bool doDifference = (e->modifiers() & Qt::ControlModifier);  // 判断是否按下了Ctrl键（用于反向选择）

		// 遍历所有要素，检查其与选择框的交集
		while (iter.nextFeature(feature)) {
			QgsGeometry featureGeometry = feature.geometry();  // 获取要素的几何体
			if (selectionGeometry.intersects(featureGeometry)) {  // 判断选择框与要素是否相交
				// 获取交集部分的几何体
				QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);

				// 如果交集部分不为空，才将其添加到选择中
				if (!intersectionGeometry.isEmpty()) {
					if (doDifference) {
						// 如果按下了Ctrl键，则移除该要素
						selectedFeatures_.remove(feature.id());
					}
					else {
						// 否则，添加该要素
						selectedFeatures_.insert(feature.id());
					}
				}
			}
		}

		// 高亮显示选中的要素（交集部分）
		highlightSelectedFeatures();

		// 重置橡皮筋线
		if (rubberBand_ != nullptr) {
			rubberBand_->reset();
		}
	}
}



// 高亮显示
void QgsMapToolSelect::highlightSelectedFeatures() {
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
			QgsGeometry selectionGeometry = QgsGeometry::fromRect(QgsRectangle(startPoint_, endPoint_));
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
				highlightRubberBand_->hide();  // 隐藏
			}
			else {
				highlightRubberBand_->show();  // 显示
			}
			});
		blinkTimer_->setInterval(500);  // 设置闪烁间隔为500ms
		blinkTimer_->start();
	}

	// 刷新画布
	mCanvas->refresh();
}



// 提取鼠标位置一定范围作为选择区域
void QgsMapToolSelect::ExpandSelectRangle(QRect& Rect, QPoint Point)
{
	int boxSize = 0;
	//如果图层不是面图元类型
	if (pLayer->geometryType() != QgsWkbTypes::PolygonGeometry) {
		boxSize = 5;
	}
	else {
		boxSize = 1;
	}
	//设置选择区域
	Rect.setLeft(Point.x() - boxSize);
	Rect.setRight(Point.x() + boxSize);
	Rect.setTop(Point.y() - boxSize);
	Rect.setBottom(Point.y() + boxSize);
}

// 将指定的设备坐标区域转换成地图坐标区域
void QgsMapToolSelect::SetRubberBand(QRect& selectRect, QgsRubberBand* pRubber)
{
	//得到当前坐标变换对象
	const QgsMapToPixel* transform = mCanvas->getCoordinateTransform();
	//将区域设备坐标转换成地图坐标
	QgsPoint ll(transform->toMapCoordinates(selectRect.left(), selectRect.bottom()));
	QgsPoint ur(transform->toMapCoordinates(selectRect.right(), selectRect.top()));
	pRubber->reset();
	//将区域的4个角点添加到QgsRubberBand对象中
	pRubber->addPoint(ll, false);
	pRubber->addPoint(QgsPoint(ur.x(), ll.y()), false);
	pRubber->addPoint(ur, false);
	pRubber->addPoint(QgsPoint(ll.x(), ur.y()), true);
}


void QgsMapToolSelect::SetEnable(bool flag)
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
void QgsMapToolSelect::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Delete) {
		// 处理删除操作
		deleteSelectedFeatures();
	}
}

// 删除并导出选中的要素
void QgsMapToolSelect::deleteSelectedFeatures()
{
	if (mCanvas == nullptr || pLayer == nullptr || selectedFeatures_.isEmpty()) {
		return;
	}

	// 获取高亮区域的几何体（即选择框的矩形）
	QgsGeometry highlightGeometry = QgsGeometry::fromRect(QgsRectangle(startPoint_, endPoint_));

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
