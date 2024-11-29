#include "CUGGIS.h"
void CUGGIS::on_actionRasterCalculator_triggered()
{
	QgsRasterCalcDialog* d = new QgsRasterCalcDialog();
	if (d->exec() != QDialog::Accepted)
	{
		return;
	}

	//调用栅格计算器
	QgsRasterCalculator rc(d->formulaString(),  //获取用户在栅格计算器对话框中输入的计算表达式
		d->outputFile(),  //获取输出文件路径
		d->outputFormat(),  //获取输出文件格式
		d->outputRectangle(),  //获取输出范围
		d->outputCrs(),   //获取输出投影
		d->numberOfColumns(),  //指定输出栅格列数
		d->numberOfRows(),  //指定输出栅格行数
		QgsRasterCalculatorEntry::rasterEntries(),  //获取与计算表达式中的栅格数据相关的条目
		QgsProject::instance()->transformContext());  // 获取QGIS项目的坐标转换上下文

	//进度显示对话框
	QProgressDialog p(tr("Calculating raster expression…"), tr("Abort"), 0, 0);
	p.setWindowTitle(tr("Raster calculator"));
	p.setWindowModality(Qt::WindowModal);
	p.setMaximum(100.0);
	//连接信号槽
	QgsFeedback feedback;
	connect(&feedback, &QgsFeedback::progressChanged, &p, &QProgressDialog::setValue);
	connect(&p, &QProgressDialog::canceled, &feedback, &QgsFeedback::cancel);
	p.show();
	//执行栅格计算
	QgsRasterCalculator::Result res = rc.processCalculation(&feedback);

	p.hide();

	//获取文件名
	QFileInfo fileInfo(d->outputFile());
	QString layerName = fileInfo.baseName();

	//挂树
	QgsRasterLayer* rasterLayer = new QgsRasterLayer(d->outputFile(), layerName);
	if (!rasterLayer->isValid()) {
		QMessageBox::critical(this, "错误", "cannot load raster");
		return;
	}
	// 将加载的栅格图层添加到项目中
	QgsProject::instance()->addMapLayer(rasterLayer);
}
void CUGGIS::on_actionBuffer_triggered()
{
	BufferWindow.show();  //显示Buffer窗口
}

void CUGGIS::on_actionSHPtoGRID_triggered()
{
	SHPtoGRIDWindow.show();//显示SHPtoGRIDW窗口
}

void CUGGIS::on_actionRectangle_triggered()
{
	// 如果 m_pSelectTool 为 nullptr，初始化它
	if (m_pSelectTool == nullptr) {
		m_pSelectTool = new QgsMapToolSelect(m_mapCanvas);  // 初始化 QgsMapToolSelect
	}

	// 存储要进行选择的图层
	QgsVectorLayer* pSelectLayer = nullptr;
	if (m_mapCanvas->layerCount() > 0) {
		pSelectLayer = (QgsVectorLayer*)m_mapCanvas->layer(0);  // 获取第一个图层
		m_pSelectTool->SetSelectLayer(pSelectLayer);  // 设置图层
	}
	m_pSelectTool->SetEnable(true);  // 启用选择工具
	m_mapCanvas->setMapTool(m_pSelectTool);  // 设置当前工具为选择工具
	m_mapCanvas->refresh();  // 刷新画布
}

void CUGGIS::on_actionCircle_triggered()
{
	// 如果 m_pCircleCut 为 nullptr，初始化它
	if (m_pCircleCut == nullptr) {
		m_pCircleCut = new CircleCut(m_mapCanvas);  // 初始化 QgsMapToolSelect
	}

	// 存储要进行选择的图层
	QgsVectorLayer* pSelectLayer2 = nullptr;
	if (m_mapCanvas->layerCount() > 0) {
		pSelectLayer2 = (QgsVectorLayer*)m_mapCanvas->layer(0);  // 获取第一个图层
		m_pCircleCut->SetSelectLayer(pSelectLayer2);  // 设置图层
	}
	m_pCircleCut->SetEnable(true);  // 启用选择工具
	m_mapCanvas->setMapTool(m_pCircleCut);  // 设置当前工具为选择工具
	m_mapCanvas->refresh();  // 刷新画布
}

void CUGGIS::on_actionRandom_triggered()
{
	// 如果 m_pPolygonCut 为 nullptr，初始化它
	if (m_pPolygonCut == nullptr) {
		m_pPolygonCut = new PolygonCut(m_mapCanvas);  // 初始化 QgsMapToolSelect
	}

	// 存储要进行选择的图层
	QgsVectorLayer* pSelectLayer2 = nullptr;
	if (m_mapCanvas->layerCount() > 0) {
		pSelectLayer2 = (QgsVectorLayer*)m_mapCanvas->layer(0);  // 获取第一个图层
		m_pPolygonCut->SetSelectLayer(pSelectLayer2);  // 设置图层
	}
	m_pPolygonCut->SetEnable(true);  // 启用选择工具
	m_mapCanvas->setMapTool(m_pPolygonCut);  // 设置当前工具为选择工具
	m_mapCanvas->refresh();  // 刷新画布
}
