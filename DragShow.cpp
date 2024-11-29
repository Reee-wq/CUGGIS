#include "CUGGIS.h"

void CUGGIS::dragEnterEvent(QDragEnterEvent* event)
{
	QList< QUrl> urs;
	urs = event->mimeData()->urls();
	// �ж��Ƿ���ڿɴ򿪵��ļ�
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
				projectOpen(fileInfo.filePath());// ��qgs��qgz�ļ�
			}
			else
			{
				addLayer(fileInfo.filePath());// ��ʸ����դ��ͼ��
			}
		}
		// ����MapCanvas
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
	// ����ļ��Ƿ����
	QFileInfo fileInfo(filepath);
	QString layerName = fileInfo.baseName(); // ��ȡͼ�����ƣ�ͨ�����ļ�����������չ����
	if (!fileInfo.exists()) {
		QMessageBox::warning(nullptr, "����", "�ļ������ڣ�");
		return;
	}

	// ��ȡ�ļ���չ��
	QString fileSuffix = fileInfo.suffix().toLower();

	bool success = false;
	if (fileSuffix == "tif" || fileSuffix == "tiff" || fileSuffix == "hdf" ||
		fileSuffix == "jpg" || fileSuffix == "jpeg" || fileSuffix == "png" || fileSuffix == "bmp") {
		// ����դ��ͼ��
		QgsRasterLayer* rasterLayer = new QgsRasterLayer(filepath, fileInfo.baseName());
		if (rasterLayer->isValid()) {
			QgsProject::instance()->addMapLayer(rasterLayer);
			success = true;
		}
		else {
			QMessageBox::warning(nullptr, "����", "�޷�����դ��ͼ�㣡");
		}
		
	}
	else if (fileSuffix == "shp") {
		// ����ʸ��ͼ�㣨Shapefile��
		QgsVectorLayer* vectorLayer = new QgsVectorLayer(filepath, fileInfo.baseName(), "ogr");
		if (vectorLayer->isValid()) {
			QgsProject::instance()->addMapLayer(vectorLayer);
			success = true;
		}
		else {
			QMessageBox::warning(nullptr, "����", "�޷�����ʸ��ͼ�㣡");
		}
	}
	else {
		QMessageBox::warning(nullptr, "����", "��֧�ֵ��ļ���ʽ��");
	}

	// ��ʾ���ؽ��
	if (success) {
		QMessageBox::information(nullptr, "�ɹ�", "ͼ���ѳɹ����أ�");
	}
}
