#include "CUGGIS.h"
#include "qgsproject.h"
#include "qgslayertree.h"

void CUGGIS::setLayersCrsToFirstLayerCrs()
{
	// 获取第一个图层的坐标系
	QgsMapLayer* firstLayer = QgsProject::instance()->mapLayers().values().first();
	if (!firstLayer) {
		qDebug() << "No layers in the project";
		return;
	}
	QgsCoordinateReferenceSystem firstCrs = firstLayer->crs();

	// 遍历所有图层，并设置它们的坐标系为第一个图层的坐标系
	const QMap<QString, QgsMapLayer*>& layers = QgsProject::instance()->mapLayers();
	for (QgsMapLayer* layer : layers) {
		if (layer->crs() != firstCrs) {
			// 设置图层的坐标系
			layer->setCrs(firstCrs, true); // 第二个参数true表示允许在QGIS中重新投影图层
		}
	}
	// 刷新所有图层以应用坐标系更改
	QList<QgsMapLayer*> layerList = QgsProject::instance()->layerTreeRoot()->layerOrder();
	for (QgsMapLayer* layer : layerList) {
		layer->triggerRepaint();
	}

	// 刷新画布以显示更新后的图层
	
	m_mapCanvas->refreshAllLayers();
	m_mapCanvas->repaint();
}
//void CUGGIS::showCoordinates(const QgsPointXY& coordinate)
//{
//	if (m_curMapLayer)
//	{
//		QgsPoint point(coordinate);
//		QgsCoordinateReferenceSystem sourceCrs = m_curMapLayer->crs();
//		QgsCoordinateReferenceSystem targetCrs = QgsCoordinateReferenceSystem::fromProj4("PROJCS[\"Custom CRS\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000],PARAMETER[\"False_Northing\",0],PARAMETER[\"Central_Meridian\",0],PARAMETER[\"Scale_Factor\",1],PARAMETER[\"Latitude_of_Origin\",0],UNIT[\"Meter\",1]]");
//
//		QgsCoordinateTransform transform(sourceCrs, targetCrs, m_mapCanvas->mapSettings().transformContext());
//		// 执行坐标转换
//		QgsPointXY transformedPoint = transform.transform(point); // 注意这里的修改
//
//		// 构建显示信息
//		QString crsInfo = QString("CRS: %1, Point: x = %2, y = %3")
//			.arg(targetCrs.toProj4())  // 显示目标坐标系的PROJ4字符串
//			.arg(transformedPoint.x(), 0, 'f', 6)  // 显示转换后的x坐标，保留6位小数
//			.arg(transformedPoint.y(), 0, 'f', 6); // 显示转换后的y坐标，保留6位小数
//		m_statusBarLabel->setText(crsInfo);
//	}
//}