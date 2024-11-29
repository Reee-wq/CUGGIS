#include "CUGGIS.h"
#include "qgsproject.h"
#include "qgslayertree.h"
#include "qgslayertreeutils.h"
#include "qgscoordinatereferencesystem.h"

void CUGGIS::setLayersCrsToLastLayerCrs()
{
    // ��ȡ��һ��ͼ�������ϵ
    QgsMapLayer* firstLayer = QgsProject::instance()->mapLayers().values().first();
    if (!firstLayer) {
        qDebug() << "No layers in the project";
        return;
    }
    QgsCoordinateReferenceSystem firstCrs = firstLayer->crs();
    qDebug()<<firstCrs.isValid();
    qDebug() << firstCrs.description();
    if (firstCrs.authid().isEmpty()) {
        qDebug() << "CRS for the first layer is empty";
    }
    else {
        qDebug() << "CRS for the first layer is:" << firstCrs.authid();
    }

    // ��������ͼ�㣬���������ǵ�����ϵΪ��һ��ͼ�������ϵ
    const QMap<QString, QgsMapLayer*>& layers = QgsProject::instance()->mapLayers();
    for (QgsMapLayer* layer : layers) {
        if (layer->crs() != firstCrs) {
            // ����ͼ�������ϵ
            layer->setCrs(firstCrs, true); // �ڶ�������true��ʾ������QGIS������ͶӰͼ��
            qDebug() << "Crs set for" << layer->name() << "to" << firstCrs.authid();
        }
    }
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
void CUGGIS::addLayersBelowLast(const QList<QgsMapLayer*>& layerList)
{
    // ��ȡͼ�����ĸ��ڵ�
    QgsLayerTree* root = QgsProject::instance()->layerTreeRoot();

    // ��ȡͼ�����е������ӽڵ�
    QList<QgsLayerTreeNode*> children = root->children();

    // �ҵ����һ��ͼ�������
    int lastLayerIndex = -1;
    for (int i = 0; i < children.size(); ++i) {
        if (children[i]->nodeType() == QgsLayerTreeNode::NodeLayer) {
            lastLayerIndex = i;
        }
    }

    // ����ͼ���б���ÿ��ͼ����ӵ���Ŀ��ͼ����
    for (QgsMapLayer* layer : layerList) {
        QgsProject::instance()->addMapLayer(layer);
        // ����ͼ����ӵ�ͼ�����У��������һ��ͼ��֮��
        if (lastLayerIndex != -1 && lastLayerIndex + 1 < root->children().size()) {
            QgsLayerTreeLayer* layerTreeLayer = root->findLayer(layer->id());
            // ���ͼ�������ж��ͼ�㣬����ͼ����뵽���һ��ͼ��֮��
            root->insertChildNode(lastLayerIndex + 1, layerTreeLayer);
        }
        else {
            // ���ͼ������û������ͼ���ֻ��һ��ͼ�㣬��ֱ�����
            root->addLayer(layer);
        }
        // �������һ��ͼ�������
        lastLayerIndex++;
    }
    //// ��ȡͼ�����ĸ��ڵ�
    //QgsLayerTree* root = QgsProject::instance()->layerTreeRoot();

    //// ��ӵ�һ��ͼ�㵽��Ŀ��ͼ����
    //if (!layerList.isEmpty())
    //{
    //    QgsMapLayer* firstLayer = layerList.first();
    //    QgsProject::instance()->addMapLayer(firstLayer);
    //    QgsLayerTreeLayer* firstLayerTreeLayer = root->findLayer(firstLayer->id());
    //    if (firstLayerTreeLayer)
    //    {
    //        int index = 0; // �ӵ�һ��ͼ��֮��ʼ����
    //        // �ӵڶ���ͼ�㿪ʼ����ÿ��ͼ����뵽���һ��ͼ����·�
    //        for (int i = 1; i < layerList.size(); ++i)
    //        {
    //            QgsMapLayer* layer = layerList[i];
    //            QgsProject::instance()->addMapLayer(layer);
    //            QgsLayerTreeLayer* layerTreeLayer = root->findLayer(layer->id());
    //            if (layerTreeLayer)
    //            {
    //                // ��ȡ��һ��ͼ��ĸ����ڵ�
    //                QgsLayerTreeNode* parent = firstLayerTreeLayer->parent();
    //                if (parent)
    //                {
    //                    // ����ͼ����뵽��һ��ͼ����·�
    //                    root->insertChildNode(index, layerTreeLayer);
    //                    index++; // ���²���λ��
    //                    
    //                }
    //            }
    //        }
    //    }
    //}
}