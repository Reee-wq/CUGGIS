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

// ���캯��
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

// ��������
PolygonCut::~PolygonCut()
{
}

// ���õ�ǰͼ��
void PolygonCut::SetSelectLayer(QgsVectorLayer* Layer)
{
    pLayer = Layer;
}

// ���������� - ��Ӷ���ζ���
void PolygonCut::canvasPressEvent(QgsMapMouseEvent* e)
{
    if (!pLayer || e->button() != Qt::LeftButton)
        return;

    // ��¼�㵽����ζ����б�
    QgsPointXY point = e->mapPoint();
    polygonVertices_.append(point);
    rubberBand_->addPoint(point);
}

// ����Ҽ� - �պ϶���β���������
void PolygonCut::canvasReleaseEvent(QgsMapMouseEvent* e)
{
    if (!pLayer || e->button() != Qt::RightButton || polygonVertices_.size() < 3)
        return;

    // �պ϶����
    polygonVertices_.append(polygonVertices_.first());
    rubberBand_->addPoint(polygonVertices_.first());

    // ���� QgsGeometry (�����Ƕ���ε�ѡ������)
    QVector<QgsPointXY> points = QVector<QgsPointXY>::fromList(polygonVertices_);
    QgsPolygonXY polygonXY;
    polygonXY.append(points);
    selectionGeometry = QgsGeometry::fromPolygonXY(polygonXY);  // �洢�����Ա��

    // ɸѡ������ڵ�Ҫ��
    QgsFeatureIterator iter = pLayer->getFeatures();
    QgsFeature feature;
    selectedFeatures_.clear();

    while (iter.nextFeature(feature)) {
        if (selectionGeometry.intersects(feature.geometry())) {
            selectedFeatures_.insert(feature.id());
        }
    }

    // ����ѡ�е�Ҫ��
    highlightSelectedFeatures();

    // ���ö����б����Ƥ��
    polygonVertices_.clear();
    rubberBand_->reset(QgsWkbTypes::PolygonGeometry);
}

// ����ѡ�е�Ҫ��
void PolygonCut::highlightSelectedFeatures()
{
    // �Ƴ�֮ǰ�ĸ�����ʾ
    if (highlightRubberBand_ != nullptr) {
        mCanvas->scene()->removeItem(highlightRubberBand_);
        delete highlightRubberBand_;
        highlightRubberBand_ = nullptr;
    }

    if (!pLayer || selectedFeatures_.isEmpty())
        return;

    // �����µ���Ƥ�������ڸ���
    highlightRubberBand_ = new QgsRubberBand(mCanvas, QgsWkbTypes::PolygonGeometry);
    highlightRubberBand_->setColor(Qt::blue);
    highlightRubberBand_->setWidth(2);

    QgsFeatureRequest request;
    request.setFilterFids(selectedFeatures_);
    QgsFeatureIterator iter = pLayer->getFeatures(request);
    QgsFeature feature;

    // ����ѡ�е�Ҫ�ز����㽻��
    while (iter.nextFeature(feature)) {
        QgsGeometry featureGeometry = feature.geometry();

        // ���㽻������
        QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);

        // ����������ֲ�Ϊ�գ�������ӵ���Ƥ������
        if (!intersectionGeometry.isEmpty()) {
            highlightRubberBand_->addGeometry(intersectionGeometry);
        }
    }

    // ���ø�����ʾ����˸Ч��
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

    blinkTimer_->start(500); // ������˸ʱ����Ϊ500ms
}

// �����¼� - Delete������ѡ��Ҫ��
void PolygonCut::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Delete) {
        saveHighlightedFeaturesToShapefile();
    }
}

// ���������ʾ��Ҫ�ص� Shapefile
void PolygonCut::saveHighlightedFeaturesToShapefile()
{
    if (!pLayer || selectedFeatures_.isEmpty()) {
        QMessageBox::warning(nullptr, "����", "δѡ���κ�Ҫ�أ�");
        return;
    }

    // ֹͣ��˸Ч��
    if (blinkTimer_ != nullptr) {
        blinkTimer_->stop();
        delete blinkTimer_;
        blinkTimer_ = nullptr;
    }

    // �����ļ�����Ի���
    QString filePath = QFileDialog::getSaveFileName(nullptr, "����ü�����ļ�", "", "Shapefile (*.shp)");

    if (filePath.isEmpty())
        return;

    // ע�����и�ʽ����
    GDALAllRegister();

    // ��ȡ Shapefile ����
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if (!poDriver) {
        QMessageBox::critical(nullptr, "����", "Shapefile ���������á�");
        return;
    }

    // �����µ�����Դ
    GDALDataset* poDS = poDriver->Create(filePath.toStdString().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
    if (!poDS) {
        QMessageBox::critical(nullptr, "����", "���� Shapefile ʧ�ܡ�");
        return;
    }

    // ȷ����������
    QgsWkbTypes::GeometryType geomType = pLayer->geometryType();
    OGRwkbGeometryType ogrType = (geomType == QgsWkbTypes::PointGeometry) ? wkbPoint :
        (geomType == QgsWkbTypes::LineGeometry) ? wkbLineString :
        wkbPolygon;

    // �����µ�ͼ��
    OGRLayer* poLayer = poDS->CreateLayer("Layer", nullptr, ogrType, nullptr);
    if (!poLayer) {
        QMessageBox::critical(nullptr, "����", "����ͼ��ʧ�ܡ�");
        GDALClose(poDS);
        return;
    }

    // �������������岢д�� Shapefile
    QgsFeatureRequest request;
    request.setFilterFids(selectedFeatures_);
    QgsFeatureIterator iter = pLayer->getFeatures(request);
    QgsFeature feature;

    while (iter.nextFeature(feature)) {
        QgsGeometry featureGeometry = feature.geometry();
        QgsGeometry intersectionGeometry = selectionGeometry.intersection(featureGeometry);

        if (!intersectionGeometry.isEmpty()) {
            // ���� OGRFeature
            OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

            // ת��������Ϊ WKT ������ OGR ������
            OGRGeometry* poGeometry = nullptr;
            std::string wkt = intersectionGeometry.asWkt().toStdString();
            OGRGeometryFactory::createFromWkt(wkt.c_str(), nullptr, &poGeometry);

            if (!poGeometry) {
                QMessageBox::warning(nullptr, "����", "������ת��ʧ�ܣ�������ǰҪ�ء�");
                OGRFeature::DestroyFeature(poFeature);
                continue;
            }

            poFeature->SetGeometry(poGeometry);

            // д��Ҫ�ص�ͼ��
            if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
                QMessageBox::warning(nullptr, "����", "д��Ҫ��ʧ�ܡ�");
            }

            // �����ڴ�
            OGRFeature::DestroyFeature(poFeature);
            OGRGeometryFactory::destroyGeometry(poGeometry);
        }
    }

    // ��������Դ
    GDALClose(poDS);

    // ��ȡ�ļ��������ص� QGIS
    QFileInfo fileInfo(filePath);
    QString outputFileName = fileInfo.baseName();

    QgsVectorLayer* newLayer = new QgsVectorLayer(filePath, outputFileName, "ogr");
    if (!newLayer->isValid()) {
        QMessageBox::warning(nullptr, "����", "�޷����� Shapefile ͼ�㡣");
        return;
    }

    // ����ͼ����ӵ� QGIS ��Ŀ
    QgsProject::instance()->addMapLayer(newLayer);

    // ��ʾ����ɹ�
    QMessageBox::information(nullptr, "����ɹ�", "Shapefile �ѳɹ����沢���ص���ͼ�С�");
}

// ��������״̬
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
