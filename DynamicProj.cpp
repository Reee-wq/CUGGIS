#include "CUGGIS.h"
#include "qgsproject.h"
#include "qgslayertree.h"

void CUGGIS::setLayersCrsToFirstLayerCrs()
{
	// ��ȡ��һ��ͼ�������ϵ
	QgsMapLayer* firstLayer = QgsProject::instance()->mapLayers().values().first();
	if (!firstLayer) {
		qDebug() << "No layers in the project";
		return;
	}
	QgsCoordinateReferenceSystem firstCrs = firstLayer->crs();

	// ��������ͼ�㣬���������ǵ�����ϵΪ��һ��ͼ�������ϵ
	const QMap<QString, QgsMapLayer*>& layers = QgsProject::instance()->mapLayers();
	for (QgsMapLayer* layer : layers) {
		if (layer->crs() != firstCrs) {
			// ����ͼ�������ϵ
			layer->setCrs(firstCrs, true); // �ڶ�������true��ʾ������QGIS������ͶӰͼ��
		}
	}
	// ˢ������ͼ����Ӧ������ϵ����
	QList<QgsMapLayer*> layerList = QgsProject::instance()->layerTreeRoot()->layerOrder();
	for (QgsMapLayer* layer : layerList) {
		layer->triggerRepaint();
	}

	// ˢ�»�������ʾ���º��ͼ��
	
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
//		// ִ������ת��
//		QgsPointXY transformedPoint = transform.transform(point); // ע��������޸�
//
//		// ������ʾ��Ϣ
//		QString crsInfo = QString("CRS: %1, Point: x = %2, y = %3")
//			.arg(targetCrs.toProj4())  // ��ʾĿ������ϵ��PROJ4�ַ���
//			.arg(transformedPoint.x(), 0, 'f', 6)  // ��ʾת�����x���꣬����6λС��
//			.arg(transformedPoint.y(), 0, 'f', 6); // ��ʾת�����y���꣬����6λС��
//		m_statusBarLabel->setText(crsInfo);
//	}
//}