#include "CUGGIS.h"
//��QGIS���ſ�
void CUGGIS::on_actionQgsStylelibMng_triggered() {
	// ��ʾ���Ա�
	static QgsStyle style;
	// �����ſ��Ƿ�Ϊ�գ����Ϊ�������Ĭ�Ϸ��ſ�
	if (style.symbolCount() == 0)
	{
		// ��ʼ�� SQLite ���ݿ�
		sqlite3_initialize();


		// �����û����ſ�·���е���ʽ
		style.load(QgsApplication::userStylePath());

	}

	// ����ʽ����Ի���
	QgsStyleManagerDialog* dlg = new QgsStyleManagerDialog(&style);

	// ���ӶԻ���ر��źŵ���Ӧ�Ĳۺ���
	connect(dlg, &QgsStyleManagerDialog::finished, this, &CUGGIS::on_styleManagerDialogFinished);

	// ��ʾ��ʽ����Ի���
	dlg->show();
}

//��ʽ�������رգ�������ſ�
void CUGGIS::on_styleManagerDialogFinished(int result) {
	static QgsStyle style;
	// ����Ի���رպ���߼�
	if (result == QDialog::Accepted) {
		style.load(QgsApplication::userStylePath());  // ���¼�����ʽ��
	}
}

//���Զ�����Ź���������
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
		SymbolModify* mSymbolModify = nullptr;  //�����޸Ĵ���
		if (!mSymbolModify) {
			mSymbolModify = new SymbolModify(this, m_curMapLayer);
		}
		mSymbolModify->show();
	}
	else {
		QgsRasterLayer* rasterLayer = qobject_cast<QgsRasterLayer*>(m_curMapLayer);
		if (rasterLayer) {
			//int res = QMessageBox::warning(this, QStringLiteral("�ֲ���ɫ"), QStringLiteral("�Ƿ��ͼ����зֲ���ɫ��"), QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
			//if (res == QMessageBox::StandardButton::Yes)
			//{
				Layering* layering = new Layering(this, m_curMapLayer);
				layering->show();
			//}
			//else {
			//	QMessageBox::warning(this, QStringLiteral("�ֲ���ɫ"), QStringLiteral("��ѡ��ʸ��ͼ�㣡"));
			//}
		}
	}

}