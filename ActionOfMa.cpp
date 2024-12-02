#include "CUGGIS.h"
#include <qgssinglesymbolrenderer.h>
#include"QgsMapToolAddFeature.h"
#include <qgsmarkersymbollayer.h>

void CUGGIS::on_actionAddPoint_triggered()
{
	if (m_curMapLayer == nullptr)
		return;

	if (QgsMapLayerType::VectorLayer != m_curMapLayer->type())
		return;

	QgsVectorLayer* layer = (QgsVectorLayer*)m_curMapLayer;
	if (layer == nullptr || !layer->isValid())
	{
		return;
	}

	// 设置当前图层
	m_mapCanvas->setLayers(QList<QgsMapLayer*>() << layer);
	m_mapCanvas->setCurrentLayer(layer);

	// 创建 AddPoint 工具实例
	m_AddPoint = new AddPoint(m_mapCanvas, layer);
	m_mapCanvas->setMapTool(m_AddPoint); // 设置地图工具为添加点工具
}

void CUGGIS::on_actionDelete_triggered()
{
	// 激活删除工具
	CustomDeleteTool* tool = new CustomDeleteTool(m_mapCanvas);
	m_mapCanvas->setMapTool(tool);

}

void CUGGIS::on_actionMove_triggered()
{
	// 激活移动工具
	CustomMoveTool* tool = new CustomMoveTool(m_mapCanvas);
	m_mapCanvas->setMapTool(tool);

}

void CUGGIS::on_actionCopy_triggered()
{
	// 激活复制工具
	CustomCopyTool* copyTool = new CustomCopyTool(m_mapCanvas);
	m_mapCanvas->setMapTool(copyTool);

}

void CUGGIS::on_actionBuffer_2_triggered()
{
	// 激活自定义的工具
	CustomBufferTool* tool = new CustomBufferTool(m_mapCanvas);
	m_mapCanvas->setMapTool(tool);

}

void CUGGIS::on_actionParallel_triggered()
{
	CustomParallelTool* tool = new CustomParallelTool(m_mapCanvas);
	m_mapCanvas->setMapTool(tool);

}

