#include "CUGGIS.h"
#include "qgsproject.h"
#include "qgslayertree.h"
#include "qgslayertreeutils.h"
#include "qgscoordinatereferencesystem.h"

void CUGGIS::setLayersCrsToLastLayerCrs()
{
    // 获取第一个图层的坐标系
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

    // 遍历所有图层，并设置它们的坐标系为第一个图层的坐标系
    const QMap<QString, QgsMapLayer*>& layers = QgsProject::instance()->mapLayers();
    for (QgsMapLayer* layer : layers) {
        if (layer->crs() != firstCrs) {
            // 设置图层的坐标系
            layer->setCrs(firstCrs, true); // 第二个参数true表示允许在QGIS中重新投影图层
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
void CUGGIS::addLayersBelowLast(const QList<QgsMapLayer*>& layerList)
{
    // 获取图层树的根节点
    QgsLayerTree* root = QgsProject::instance()->layerTreeRoot();

    // 获取图层树中的所有子节点
    QList<QgsLayerTreeNode*> children = root->children();

    // 找到最后一个图层的索引
    int lastLayerIndex = -1;
    for (int i = 0; i < children.size(); ++i) {
        if (children[i]->nodeType() == QgsLayerTreeNode::NodeLayer) {
            lastLayerIndex = i;
        }
    }

    // 遍历图层列表，将每个图层添加到项目和图层树
    for (QgsMapLayer* layer : layerList) {
        QgsProject::instance()->addMapLayer(layer);
        // 将新图层添加到图层树中，放在最后一个图层之后
        if (lastLayerIndex != -1 && lastLayerIndex + 1 < root->children().size()) {
            QgsLayerTreeLayer* layerTreeLayer = root->findLayer(layer->id());
            // 如果图层树中有多个图层，将新图层插入到最后一个图层之后
            root->insertChildNode(lastLayerIndex + 1, layerTreeLayer);
        }
        else {
            // 如果图层树中没有其他图层或只有一个图层，则直接添加
            root->addLayer(layer);
        }
        // 更新最后一个图层的索引
        lastLayerIndex++;
    }
    //// 获取图层树的根节点
    //QgsLayerTree* root = QgsProject::instance()->layerTreeRoot();

    //// 添加第一个图层到项目和图层树
    //if (!layerList.isEmpty())
    //{
    //    QgsMapLayer* firstLayer = layerList.first();
    //    QgsProject::instance()->addMapLayer(firstLayer);
    //    QgsLayerTreeLayer* firstLayerTreeLayer = root->findLayer(firstLayer->id());
    //    if (firstLayerTreeLayer)
    //    {
    //        int index = 0; // 从第一个图层之后开始插入
    //        // 从第二个图层开始，将每个图层插入到最后一个图层的下方
    //        for (int i = 1; i < layerList.size(); ++i)
    //        {
    //            QgsMapLayer* layer = layerList[i];
    //            QgsProject::instance()->addMapLayer(layer);
    //            QgsLayerTreeLayer* layerTreeLayer = root->findLayer(layer->id());
    //            if (layerTreeLayer)
    //            {
    //                // 获取第一个图层的父级节点
    //                QgsLayerTreeNode* parent = firstLayerTreeLayer->parent();
    //                if (parent)
    //                {
    //                    // 将新图层插入到第一个图层的下方
    //                    root->insertChildNode(index, layerTreeLayer);
    //                    index++; // 更新插入位置
    //                    
    //                }
    //            }
    //        }
    //    }
    //}
}