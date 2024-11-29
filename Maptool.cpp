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
	// 使用 QgsMapToolPan 设置平移工具
	QgsMapToolPan* panTool = new QgsMapToolPan(m_mapCanvas);
	m_mapCanvas->setMapTool(panTool);  // 设置地图为平移模式
	m_mapCanvas->setCursor(Qt::OpenHandCursor); // 显式设置地图视图的光标
}
//void CUGGIS::resizeEvent(QResizeEvent* event)
//{
//	// 确保 m_mapCanvas 的大小会随窗口大小变化而变化
//	if (m_mapCanvas) {
//		m_mapCanvas->resize(event->size());  // 更新 m_mapCanvas 的大小
//	}
//
//	QMainWindow::resizeEvent(event);  // 保持其他事件的正常处理
//}

void CUGGIS::on_actionExitPan_triggered()
{
	// 使用自定义的默认工具
	MyDefaultMapTool* defaultTool = new MyDefaultMapTool(m_mapCanvas);
	m_mapCanvas->setMapTool(defaultTool); // 设置为无操作工具
}
