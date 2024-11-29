#include "CUGGIS.h"	
#include <qfiledialog.h>
#include <QMessageBox>
#include "qgsproject.h"

void CUGGIS::on_actionNewProject_triggered() 
{
	// 创建一个新的项目
	QgsProject::instance()->clear();

	// 重置项目文件名，表示当前没有关联的文件
	QgsProject::instance()->setFileName("");
}
void CUGGIS::on_actionOpenProject_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, QStringLiteral("选择工程文件"), "", "QGIS project (*.qgs)");
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
	// 假设您已经有一个项目的文件路径
	QString currentProjectPath = QgsProject::instance()->fileName();
	if (currentProjectPath.isEmpty()) {
		// 如果当前没有打开的项目文件，调用另存为
		on_actionSaveAsProject_triggered();
	}
	else {
		// 如果有打开的项目文件，直接保存
		saveProject(currentProjectPath);
	}
}
void CUGGIS::on_actionSaveAsProject_triggered()
{
	QString filename = QFileDialog::getSaveFileName(this, QStringLiteral("工程另存为"), "", "QGIS project (*.qgs)");
	QFileInfo file(filename);
	QgsProject::instance()->write(filename);
}

void CUGGIS::saveProject(const QString& filePath)
{
	// 保存当前项目到指定的文件路径
	if (QgsProject::instance()->write(filePath)) {
		QMessageBox::information(this, tr("Success"), tr("Project saved successfully."));
		// 更新当前项目的文件路径
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