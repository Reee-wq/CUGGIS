#include "CUGGIS.h"
#include "MyDefaultMapTool.h"
#include <QgsMapToolPan.h>
#include "MyDefaultMapTool.h"
void CUGGIS::on_actionZoomIn_triggered()
{
	m_mapCanvas->setMapTool(m_zoomInTool);
}

void CUGGIS::on_actionZoomOut_triggered()
{
	m_mapCanvas->setMapTool(m_zoomOutTool);
}

void CUGGIS::on_actionPan_triggered()
{
	// ʹ�� QgsMapToolPan ����ƽ�ƹ���
	QgsMapToolPan* panTool = new QgsMapToolPan(m_mapCanvas);
	m_mapCanvas->setMapTool(panTool);  // ���õ�ͼΪƽ��ģʽ
	m_mapCanvas->setCursor(Qt::OpenHandCursor); // ��ʽ���õ�ͼ��ͼ�Ĺ��
}
//void CUGGIS::resizeEvent(QResizeEvent* event)
//{
//	// ȷ�� m_mapCanvas �Ĵ�С���洰�ڴ�С�仯���仯
//	if (m_mapCanvas) {
//		m_mapCanvas->resize(event->size());  // ���� m_mapCanvas �Ĵ�С
//	}
//
//	QMainWindow::resizeEvent(event);  // ���������¼�����������
//}

void CUGGIS::on_actionExitPan_triggered()
{
	// ʹ���Զ����Ĭ�Ϲ���
	MyDefaultMapTool* defaultTool = new MyDefaultMapTool(m_mapCanvas);
	m_mapCanvas->setMapTool(defaultTool); // ����Ϊ�޲�������
}
