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

	// ���õ�ǰͼ��
	m_mapCanvas->setLayers(QList<QgsMapLayer*>() << layer);
	m_mapCanvas->setCurrentLayer(layer);

	// ���� AddPoint ����ʵ��
	m_AddPoint = new AddPoint(m_mapCanvas, layer);
	m_mapCanvas->setMapTool(m_AddPoint); // ���õ�ͼ����Ϊ��ӵ㹤��
}

void CUGGIS::on_actionDelete_triggered()
{
	// ����ɾ������
	CustomDeleteTool* tool = new CustomDeleteTool(m_mapCanvas);
	m_mapCanvas->setMapTool(tool);

}

void CUGGIS::on_actionMove_triggered()
{
	// �����ƶ�����
	CustomMoveTool* tool = new CustomMoveTool(m_mapCanvas);
	m_mapCanvas->setMapTool(tool);

}

void CUGGIS::on_actionCopy_triggered()
{
	// ����ƹ���
	CustomCopyTool* copyTool = new CustomCopyTool(m_mapCanvas);
	m_mapCanvas->setMapTool(copyTool);

}

void CUGGIS::on_actionBuffer_2_triggered()
{
	// �����Զ���Ĺ���
	CustomBufferTool* tool = new CustomBufferTool(m_mapCanvas);
	m_mapCanvas->setMapTool(tool);

}

void CUGGIS::on_actionParallel_triggered()
{
	CustomParallelTool* tool = new CustomParallelTool(m_mapCanvas);
	m_mapCanvas->setMapTool(tool);

}

