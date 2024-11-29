/**************************Բ�βü�����*********************
*****************************************************************/

#include "CircleCut.h"
#include "qgsgeometry.h"
#include <QgsProject.h>
#include <QMessageBox>

//�������������
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

// ���õ�ǰ��ѡ��(�)��ͼ��
void CircleCut::SetSelectLayer(QgsVectorLayer* Layer)
{
    pLayer = Layer;
}

// ������갴��
void CircleCut::canvasPressEvent(QgsMapMouseEvent* e)
{
    if (mCanvas == nullptr || pLayer == nullptr) return;
    if (e->button() == Qt::LeftButton) {
        startPoint_ = e->mapPoint(); //�洢ѡ������ʼ��
        rubberBand_->setColor(Qt::red); //��Ƥ����ɫ
        rubberBand_->setWidth(2); //�߿�
        rubberBand_->addPoint(startPoint_);
        selecting_ = true;
    }
}

// ����ƶ�
void CircleCut::canvasMoveEvent(QgsMapMouseEvent* e)
{
    if (mCanvas == nullptr || pLayer == nullptr || !selecting_) return;
    endPoint_ = e->mapPoint(); // ���½�����

    // ������Ƥ�����Բ��ѡ������
    rubberBand_->reset();

    // ���� startPoint_ �� endPoint_ �� x �� y ֵ�� SetRubberBand
    SetRubberBand(startPoint_.x(), startPoint_.y(), endPoint_.x(), endPoint_.y(), rubberBand_);
}


// ��������ͷ�
void CircleCut::canvasReleaseEvent(QgsMapMouseEvent* e) {
    if (mCanvas == nullptr || pLayer == nullptr || !selecting_) return;
    if (e->button() == Qt::LeftButton) {
        selecting_ = false;
        endPoint_ = e->mapPoint();

        // ʹ�� fromPointXY ����������
        double radius = startPoint_.distance(endPoint_);
        QgsGeometry selectionGeometry = QgsGeometry::fromPointXY(QgsPointXY(startPoint_)).buffer(radius, 0);

        // ʹ�� setFilterRect ���� setFilterGeometry
        QgsFeatureRequest request;
        request.setFilterRect(selectionGeometry.boundingBox());  // ʹ�ð�Χ�н��й���

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

// ������ʾ
void CircleCut::highlightSelectedFeatures() {
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
            QgsGeometry selectionGeometry = QgsGeometry::fromPointXY(QgsPointXY(startPoint_)).buffer(startPoint_.distance(endPoint_), 20);  // ����Բ�λ�����
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
                highlightRubberBand_->hide();
            }
            else {
                highlightRubberBand_->show();
            }
            });

        blinkTimer_->start(500);  // ������˸ʱ����Ϊ500ms
    }
}


// ����Բ��ѡ���
void CircleCut::SetRubberBand(double startX, double startY, double endX, double endY, QgsRubberBand* rubberBand) {
    // ������������Ƥ��
    QgsPointXY startPoint(startX, startY);
    QgsPointXY endPoint(endX, endY);

    // ����������Ը�����Ҫ����Բ�Ρ��߶ε�
    rubberBand->addPoint(startPoint);
    rubberBand->addPoint(endPoint);  // ʾ��������ʵ�������޸�
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

// ���������¼�
void CircleCut::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Delete) {
        // ����ɾ������
        deleteSelectedFeatures();
    }
}

// ɾ��������ѡ�е�Ҫ��
void CircleCut::deleteSelectedFeatures()
{
    if (mCanvas == nullptr || pLayer == nullptr || selectedFeatures_.isEmpty()) {
        return;
    }

    // ����һ��Բ��ѡ��򼸺��壬ʹ�� QgsPointXY �� buffer ������Բ��
    double radius = startPoint_.distance(endPoint_); // �뾶Ϊ��ʼ��ͽ�����֮��ľ���
    QgsPointXY point(startPoint_); // �����㼸��
    QgsGeometry highlightGeometry = QgsGeometry::fromPointXY(point).buffer(radius, 20); // 20 �ǻ������ķֶ������ɸ����������

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

