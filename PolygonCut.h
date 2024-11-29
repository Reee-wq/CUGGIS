#ifndef POLYGONCUT_H
#define POLYGONCUT_H
#pragma execution_character_set("utf-8")
#include <qgsmaptool.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsrubberband.h>
#include <QgsVectorFileWriter.h>
#include <QgsMapMouseEvent.h>
#include <QPoint>
#include <QMessageBox>
#include <QKeyEvent>
#include <QFileDialog>
#include <QTimer>
#include <QSet>

#include <gdal.h>
#include <ogr_api.h>
#include <ogr_geometry.h>
#include <ogr_feature.h>
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>

class PolygonCut : public QgsMapTool
{
    Q_OBJECT
public:
    PolygonCut(QgsMapCanvas*);
    ~PolygonCut(void);

    void SetSelectLayer(QgsVectorLayer*);
    virtual void canvasReleaseEvent(QgsMapMouseEvent* e);
    virtual void canvasPressEvent(QgsMapMouseEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    void SetEnable(bool);

private:
    QgsVectorLayer* pLayer;
    QgsRubberBand* rubberBand_ = nullptr;
    QList<QgsPointXY> polygonVertices_;
    QSet<QgsFeatureId> selectedFeatures_;
    QgsRubberBand* highlightRubberBand_ = nullptr;
    bool StatusFlag;
    QgsGeometry selectionGeometry;
    void highlightSelectedFeatures();
    void saveHighlightedFeaturesToShapefile();

    QTimer* blinkTimer_ = nullptr;
};

#endif
