#include "CUGGIS.h"
//打开QGIS符号库
void CUGGIS::on_actionQgsStylelibMng_triggered() {
	// 显示属性表
	static QgsStyle style;
	// 检查符号库是否为空，如果为空则加载默认符号库
	if (style.symbolCount() == 0)
	{
		// 初始化 SQLite 数据库
		sqlite3_initialize();


		// 加载用户符号库路径中的样式
		style.load(QgsApplication::userStylePath());

	}

	// 打开样式管理对话框
	QgsStyleManagerDialog* dlg = new QgsStyleManagerDialog(&style);

	// 连接对话框关闭信号到相应的槽函数
	connect(dlg, &QgsStyleManagerDialog::finished, this, &CUGGIS::on_styleManagerDialogFinished);

	// 显示样式管理对话框
	dlg->show();
}

//样式管理器关闭，保存符号库
void CUGGIS::on_styleManagerDialogFinished(int result) {
	static QgsStyle style;
	// 处理对话框关闭后的逻辑
	if (result == QDialog::Accepted) {
		style.load(QgsApplication::userStylePath());  // 重新加载样式库
	}
}

//打开自定义符号管理器窗口
void CUGGIS::on_actionSelfStylelibMng_triggered() {
	if (!mSymbolManager) {
		mSymbolManager = new SymbolManager(this);
	}
	mSymbolManager->show();
}

void  CUGGIS::onLayerTreeItemDoubleClicked() {


	QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>(m_curMapLayer);
	if (vectorLayer)
	{
		SymbolModify* mSymbolModify = nullptr;  //符号修改窗口
		if (!mSymbolModify) {
			mSymbolModify = new SymbolModify(this, m_curMapLayer);
		}
		mSymbolModify->show();
	}
	else {
		QgsRasterLayer* rasterLayer = qobject_cast<QgsRasterLayer*>(m_curMapLayer);
		if (rasterLayer) {
			//int res = QMessageBox::warning(this, QStringLiteral("分层设色"), QStringLiteral("是否对图层进行分层设色？"), QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
			//if (res == QMessageBox::StandardButton::Yes)
			//{
				Layering* layering = new Layering(this, m_curMapLayer);
				layering->show();
			//}
			//else {
			//	QMessageBox::warning(this, QStringLiteral("分层设色"), QStringLiteral("请选择矢量图层！"));
			//}
		}
	}

}