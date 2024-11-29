#include "CUGGIS.h"	
#include <qfiledialog.h>
#include <QMessageBox>
#include "qgsproject.h"

void CUGGIS::on_actionNewProject_triggered() 
{
	// ����һ���µ���Ŀ
	QgsProject::instance()->clear();

	// ������Ŀ�ļ�������ʾ��ǰû�й������ļ�
	QgsProject::instance()->setFileName("");
}
void CUGGIS::on_actionOpenProject_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, QStringLiteral("ѡ�񹤳��ļ�"), "", "QGIS project (*.qgs)");
	QFileInfo fi(filename);
	if (!fi.exists())
	{
		return;
	}

	QgsProject::instance()->read(filename);

	m_curMapLayer = QgsProject::instance()->mapLayer(0);
}
void CUGGIS::on_actionSaveProject_triggered()
{
	// �������Ѿ���һ����Ŀ���ļ�·��
	QString currentProjectPath = QgsProject::instance()->fileName();
	if (currentProjectPath.isEmpty()) {
		// �����ǰû�д򿪵���Ŀ�ļ����������Ϊ
		on_actionSaveAsProject_triggered();
	}
	else {
		// ����д򿪵���Ŀ�ļ���ֱ�ӱ���
		saveProject(currentProjectPath);
	}
}
void CUGGIS::on_actionSaveAsProject_triggered()
{
	QString filename = QFileDialog::getSaveFileName(this, QStringLiteral("�������Ϊ"), "", "QGIS project (*.qgs)");
	QFileInfo file(filename);
	QgsProject::instance()->write(filename);
}

void CUGGIS::saveProject(const QString& filePath)
{
	// ���浱ǰ��Ŀ��ָ�����ļ�·��
	if (QgsProject::instance()->write(filePath)) {
		QMessageBox::information(this, tr("Success"), tr("Project saved successfully."));
		// ���µ�ǰ��Ŀ���ļ�·��
		QgsProject::instance()->setFileName(filePath);
	}
	else {
		QMessageBox::warning(this, tr("Error"), tr("Failed to save the project."));
	}
}
void CUGGIS::on_actionExit_triggered()
{
    	close();
}