/**************************ѡ��ͼ��Ҫ�ع���*********************
*****************************************************************/
#include "qgsmaptoolselect.h"
#include "qgsgeometry.h"
#include <QgsProject.h>
#include <QMessageBox>

//�������������
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

// ���õ�ǰ��ѡ��(�)��ͼ��
void QgsMapToolSelect::SetSelectLayer(QgsVectorLayer* Layer)
{
	pLayer = Layer;
}


// ������갴��
void QgsMapToolSelect::canvasPressEvent(QgsMapMouseEvent* e)
{
	if (mCanvas == nullptr || pLayer == nullptr) return;
	if (e->button() == Qt::LeftButton) {
		startPoint_ = e->mapPoint(); //�洢ѡ������ʼ��
		//rubberBand_(mCanvas, pLayer->geometryType());
		rubberBand_->setColor(Qt::red); //��Ƥ����ɫ
		rubberBand_->setWidth(2); //�߿�
		rubberBand_->addPoint(startPoint_);
		selecting_ = true;
		//delete rubberBand_;
		//rubberBand_ = nullptr;
	}
}

// ����ƶ�
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

// ��������ͷ�
void QgsMapToolSelect::canvasReleaseEvent(QgsMapMouseEvent* e) {
	if (mCanvas == nullptr || pLayer == nullptr || !selecting_) return;
	if (e->button() == Qt::LeftButton) {
		selecting_ = false;  // ֹͣѡ��
		endPoint_ = e->mapPoint();  // ��ȡ����ɿ�ʱ�Ľ�����

		// ����ѡ���ľ��μ�����
		QgsRectangle rect(startPoint_, endPoint_);
		QgsGeometry selectionGeometry = QgsGeometry::fromRect(rect);  // ������ת��Ϊ���ζ���

		// ���ò�ѯ������ֻ��ȡ����������ཻ��Ҫ��
		QgsFeatureRequest request;
		request.setFilterRect(rect);  // ʹ�þ���������Ϊ�������������Ч��

		QgsFeatureIterator iter = pLayer->getFeatures(request);  // ��ȡ������ѡ����ཻ��Ҫ��
		QgsFeature feature;
		bool doDifference = (e->modifiers() & Qt::ControlModifier);  // �ж��Ƿ�����Ctrl�������ڷ���ѡ��

		// ��������Ҫ�أ��������ѡ���Ľ���
		while (iter.nextFeature(feature)) {
			QgsGeometry featureGeometry = feature.geometry();  // ��ȡҪ�صļ�����
			if (selectionGeometry.intersects(featureGeometry)) {  // �ж�ѡ�����Ҫ���Ƿ��ཻ
				// ��ȡ�������ֵļ�����
				QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);

				// ����������ֲ�Ϊ�գ��Ž�����ӵ�ѡ����
				if (!intersectionGeometry.isEmpty()) {
					if (doDifference) {
						// ���������Ctrl�������Ƴ���Ҫ��
						selectedFeatures_.remove(feature.id());
					}
					else {
						// ������Ӹ�Ҫ��
						selectedFeatures_.insert(feature.id());
					}
				}
			}
		}

		// ������ʾѡ�е�Ҫ�أ��������֣�
		highlightSelectedFeatures();

		// ������Ƥ����
		if (rubberBand_ != nullptr) {
			rubberBand_->reset();
		}
	}
}



// ������ʾ
void QgsMapToolSelect::highlightSelectedFeatures() {
	// �Ƴ�֮ǰ�ĸ�����ʾ������еĻ���
	if (highlightRubberBand_ != nullptr) {
		mCanvas->scene()->removeItem(highlightRubberBand_);
		delete highlightRubberBand_;
		highlightRubberBand_ = nullptr;
	}

	if (!selectedFeatures_.isEmpty()) {
		// ����һ���µ���Ƥ������������ʾѡ�еĲ���
		highlightRubberBand_ = new QgsRubberBand(mCanvas, pLayer->geometryType());
		highlightRubberBand_->setColor(Qt::blue);  // ���ø�����ɫ
		highlightRubberBand_->setWidth(2);  // ���ñ߿���

		// �������б�ѡ�е�Ҫ��ID����ȡ������ѡ��򽻼����ֵļ�����
		QgsFeatureRequest request;
		request.setFilterFids(selectedFeatures_);
		QgsFeatureIterator iter = pLayer->getFeatures(request);
		QgsFeature feature;
		while (iter.nextFeature(feature)) {
			QgsGeometry featureGeometry = feature.geometry();  // ��ȡҪ�ؼ�����
			QgsGeometry selectionGeometry = QgsGeometry::fromRect(QgsRectangle(startPoint_, endPoint_));
			// ����ѡ�����Ҫ�صĽ�������
			QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);

			// ����������ֲ�Ϊ�գ�������ӵ���Ƥ������
			if (!intersectionGeometry.isEmpty()) {
				highlightRubberBand_->addGeometry(intersectionGeometry);
			}
		}

		// ���ø�����ʾ����˸Ч������ѡ��
		if (blinkTimer_ != nullptr) {
			blinkTimer_->stop();  // ֹͣ��ʱ��
			delete blinkTimer_;
			blinkTimer_ = nullptr;
		}

		blinkTimer_ = new QTimer(this);
		connect(blinkTimer_, &QTimer::timeout, this, [this]() {
			if (highlightRubberBand_->isVisible()) {
				highlightRubberBand_->hide();  // ����
			}
			else {
				highlightRubberBand_->show();  // ��ʾ
			}
			});
		blinkTimer_->setInterval(500);  // ������˸���Ϊ500ms
		blinkTimer_->start();
	}

	// ˢ�»���
	mCanvas->refresh();
}



// ��ȡ���λ��һ����Χ��Ϊѡ������
void QgsMapToolSelect::ExpandSelectRangle(QRect& Rect, QPoint Point)
{
	int boxSize = 0;
	//���ͼ�㲻����ͼԪ����
	if (pLayer->geometryType() != QgsWkbTypes::PolygonGeometry) {
		boxSize = 5;
	}
	else {
		boxSize = 1;
	}
	//����ѡ������
	Rect.setLeft(Point.x() - boxSize);
	Rect.setRight(Point.x() + boxSize);
	Rect.setTop(Point.y() - boxSize);
	Rect.setBottom(Point.y() + boxSize);
}

// ��ָ�����豸��������ת���ɵ�ͼ��������
void QgsMapToolSelect::SetRubberBand(QRect& selectRect, QgsRubberBand* pRubber)
{
	//�õ���ǰ����任����
	const QgsMapToPixel* transform = mCanvas->getCoordinateTransform();
	//�������豸����ת���ɵ�ͼ����
	QgsPoint ll(transform->toMapCoordinates(selectRect.left(), selectRect.bottom()));
	QgsPoint ur(transform->toMapCoordinates(selectRect.right(), selectRect.top()));
	pRubber->reset();
	//�������4���ǵ���ӵ�QgsRubberBand������
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

// ���������¼�
void QgsMapToolSelect::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Delete) {
		// ����ɾ������
		deleteSelectedFeatures();
	}
}

// ɾ��������ѡ�е�Ҫ��
void QgsMapToolSelect::deleteSelectedFeatures()
{
	if (mCanvas == nullptr || pLayer == nullptr || selectedFeatures_.isEmpty()) {
		return;
	}

	// ��ȡ��������ļ����壨��ѡ���ľ��Σ�
	QgsGeometry highlightGeometry = QgsGeometry::fromRect(QgsRectangle(startPoint_, endPoint_));

	// ����һ���µ��б����ڴ洢�ü���ļ�����
	QList<QgsGeometry> geometriesToSave;

	// ��ȡ��ǰͼ��ļ�������
	QgsWkbTypes::GeometryType geomType = pLayer->geometryType();

	// ����ѡ�е�����Ҫ��
	QgsFeatureIterator iter = pLayer->getFeatures(QgsFeatureRequest().setFilterFids(selectedFeatures_));
	QgsFeature feature;
	while (iter.nextFeature(feature)) {
		QgsGeometry featureGeometry = feature.geometry();

		// �жϼ������Ͳ�������Ӧ�Ĳü�
		if (geomType == QgsWkbTypes::PointGeometry) {
			// �㣺�����Ƿ���ѡ�����
			if (highlightGeometry.contains(featureGeometry)) {
				geometriesToSave.append(featureGeometry); // ��ֱ����ӵ������б���
			}
		}
		else if (geomType == QgsWkbTypes::LineGeometry) {
			// �ߣ���������ѡ���Ľ���
			QgsGeometry intersectionGeometry = featureGeometry.intersection(highlightGeometry);
			if (!intersectionGeometry.isEmpty()) {
				geometriesToSave.append(intersectionGeometry); // ���潻������
			}
		}
		else if (geomType == QgsWkbTypes::PolygonGeometry) {
			// �棺��������ѡ���Ľ���
			QgsGeometry intersectionGeometry = featureGeometry.intersection(highlightGeometry);
			if (!intersectionGeometry.isEmpty()) {
				geometriesToSave.append(intersectionGeometry); // ���潻������
			}
		}
	}

	// ����м����屻�ü�������Ϊ�µ� Shapefile
	if (!geometriesToSave.isEmpty()) {
		if (highlightRubberBand_) {
			highlightRubberBand_->setVisible(false);  // ȷ���������ɼ�
			mCanvas->scene()->removeItem(highlightRubberBand_);
			delete highlightRubberBand_;
			highlightRubberBand_ = nullptr;
		}

		if (blinkTimer_) {
			blinkTimer_->stop();  // ֹͣ��ʱ��
			delete blinkTimer_;
			blinkTimer_ = nullptr;  // ��������ָ��
		}

		// �����ļ�����Ի������û�ѡ�񱣴�·��
		QString filePath = QFileDialog::getSaveFileName(nullptr, "����ü�����ļ�", "", "Shapefile (*.shp)");

		if (!filePath.isEmpty()) {
			// ʹ�� GDAL �����µ� Shapefile ����Դ
			GDALAllRegister(); // ע�����и�ʽ����
			GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
			if (poDriver == nullptr) {
				QMessageBox::critical(nullptr, "Error", "Shapefile driver not available.");
				return;
			}

			// �����µ�����Դ
			GDALDataset* poDS = poDriver->Create(filePath.toStdString().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
			if (poDS == nullptr) {
				QMessageBox::critical(nullptr, "Error", "Failed to create shapefile.");
				return;
			}

			// �����µ�ͼ��
			OGRwkbGeometryType ogrType = (geomType == QgsWkbTypes::PointGeometry) ? wkbPoint :
				(geomType == QgsWkbTypes::LineGeometry) ? wkbLineString : wkbPolygon;
			OGRLayer* poLayer = poDS->CreateLayer("Layer", nullptr, ogrType, nullptr);
			if (poLayer == nullptr) {
				QMessageBox::critical(nullptr, "Error", "Failed to create layer.");
				GDALClose(poDS);
				return;
			}

			// д�뼸����
			for (const QgsGeometry& geometry : geometriesToSave) {
				OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

				// ���ü�����
				OGRGeometry* poGeometry = nullptr;
				std::string wkt = geometry.asWkt().toStdString();
				OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &poGeometry);
				if (poGeometry == nullptr) {
					QMessageBox::warning(nullptr, "Warning", "Failed to convert geometry.");
					continue;
				}
				poFeature->SetGeometry(poGeometry);

				// д��Ҫ�ص�ͼ��
				if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
					QMessageBox::warning(nullptr, "Warning", "Failed to write feature.");
				}

				// ���������Ҫ��
				OGRFeature::DestroyFeature(poFeature);
				OGRGeometryFactory::destroyGeometry(poGeometry);
			}

			// �������ݼ�
			GDALClose(poDS);

			//��ȡ�ļ���
			QFileInfo fileInfo(filePath);
			QString outputFileName = fileInfo.baseName();

			// ���� QgsVectorLayer  
			QgsVectorLayer* pointLayer = new QgsVectorLayer(filePath, outputFileName, "ogr");
			if (!pointLayer->isValid()) {
				QMessageBox::warning(nullptr, "����", "�޷����� Shapefile ͼ�㡣");
				return;
			}
			//��ת�����shp�ļ���ͼ����
			QgsProject::instance()->addMapLayer(pointLayer);

			// ��ʾ����ɹ�
			QMessageBox::information(nullptr, "Export Successful", "Shapefile has been saved successfully.");
		}
	}
}
