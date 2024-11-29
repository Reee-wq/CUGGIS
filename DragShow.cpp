#include "CUGGIS.h"

void CUGGIS::dragEnterEvent(QDragEnterEvent* event)
{
	QList< QUrl> urs;
	urs = event->mimeData()->urls();
	// 判断是否存在可打开的文件
	for (int i = 0; i < urs.size(); i++)
	{
		QFileInfo fileInfo(urs[i].toLocalFile());
		if (formatList.contains(fileInfo.suffix()))
		{
			event->acceptProposedAction();
			break;
		}
		event->ignore();
	}
}


void CUGGIS::dropEvent(QDropEvent* event) 
{
	QList< QUrl> urs;
	urs = event->mimeData()->urls();
	for (int i = 0; i < urs.size(); i++)
	{
		QFileInfo fileInfo(urs[i].toLocalFile());
		if (formatList.contains(fileInfo.suffix()))
		{
			if ("qgs" == fileInfo.suffix() || "qgz" == fileInfo.suffix())
			{
				projectOpen(fileInfo.filePath());// 打开qgs与qgz文件
			}
			else
			{
				addLayer(fileInfo.filePath());// 打开矢量或栅格图层
			}
		}
		// 更新MapCanvas
		if (m_mapCanvas)
		{
			m_mapCanvas->setExtent(m_mapCanvas->mapSettings().extent());
			m_mapCanvas->refresh();
		}
	}
}

void CUGGIS::projectOpen(const QString& filepath)
{
	
	QFileInfo fi(filepath);
	
	if (!fi.exists())
	{
		return;
	}

	QgsProject::instance()->read(filepath);

	m_curMapLayer = QgsProject::instance()->mapLayer(0);
}

void CUGGIS::addLayer(const QString& filepath)
{
	// 检查文件是否存在
	QFileInfo fileInfo(filepath);
	QString layerName = fileInfo.baseName(); // 获取图层名称，通常是文件名（不带扩展名）
	if (!fileInfo.exists()) {
		QMessageBox::warning(nullptr, "错误", "文件不存在！");
		return;
	}

	// 获取文件扩展名
	QString fileSuffix = fileInfo.suffix().toLower();

	bool success = false;
	if (fileSuffix == "tif" || fileSuffix == "tiff" || fileSuffix == "hdf" ||
		fileSuffix == "jpg" || fileSuffix == "jpeg" || fileSuffix == "png" || fileSuffix == "bmp") {
		// 加载栅格图层
		QgsRasterLayer* rasterLayer = new QgsRasterLayer(filepath, fileInfo.baseName());
		if (rasterLayer->isValid()) {
			QgsProject::instance()->addMapLayer(rasterLayer);
			success = true;
		}
		else {
			QMessageBox::warning(nullptr, "错误", "无法加载栅格图层！");
		}
		
	}
	else if (fileSuffix == "shp") {
		// 加载矢量图层（Shapefile）
		QgsVectorLayer* vectorLayer = new QgsVectorLayer(filepath, fileInfo.baseName(), "ogr");
		if (vectorLayer->isValid()) {
			QgsProject::instance()->addMapLayer(vectorLayer);
			success = true;
		}
		else {
			QMessageBox::warning(nullptr, "错误", "无法加载矢量图层！");
		}
	}
	else {
		QMessageBox::warning(nullptr, "错误", "不支持的文件格式！");
	}

	// 提示加载结果
	if (success) {
		QMessageBox::information(nullptr, "成功", "图层已成功加载！");
	}
}
