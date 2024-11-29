#include "CUGGIS.h"
void CUGGIS::on_actionRasterCalculator_triggered()
{
	QgsRasterCalcDialog* d = new QgsRasterCalcDialog();
	if (d->exec() != QDialog::Accepted)
	{
		return;
	}

	//����դ�������
	QgsRasterCalculator rc(d->formulaString(),  //��ȡ�û���դ��������Ի���������ļ�����ʽ
		d->outputFile(),  //��ȡ����ļ�·��
		d->outputFormat(),  //��ȡ����ļ���ʽ
		d->outputRectangle(),  //��ȡ�����Χ
		d->outputCrs(),   //��ȡ���ͶӰ
		d->numberOfColumns(),  //ָ�����դ������
		d->numberOfRows(),  //ָ�����դ������
		QgsRasterCalculatorEntry::rasterEntries(),  //��ȡ�������ʽ�е�դ��������ص���Ŀ
		QgsProject::instance()->transformContext());  // ��ȡQGIS��Ŀ������ת��������

	//������ʾ�Ի���
	QProgressDialog p(tr("Calculating raster expression��"), tr("Abort"), 0, 0);
	p.setWindowTitle(tr("Raster calculator"));
	p.setWindowModality(Qt::WindowModal);
	p.setMaximum(100.0);
	//�����źŲ�
	QgsFeedback feedback;
	connect(&feedback, &QgsFeedback::progressChanged, &p, &QProgressDialog::setValue);
	connect(&p, &QProgressDialog::canceled, &feedback, &QgsFeedback::cancel);
	p.show();
	//ִ��դ�����
	QgsRasterCalculator::Result res = rc.processCalculation(&feedback);

	p.hide();

	//��ȡ�ļ���
	QFileInfo fileInfo(d->outputFile());
	QString layerName = fileInfo.baseName();

	//����
	QgsRasterLayer* rasterLayer = new QgsRasterLayer(d->outputFile(), layerName);
	if (!rasterLayer->isValid()) {
		QMessageBox::critical(this, "����", "cannot load raster");
		return;
	}
	// �����ص�դ��ͼ����ӵ���Ŀ��
	QgsProject::instance()->addMapLayer(rasterLayer);
}
void CUGGIS::on_actionBuffer_triggered()
{
	BufferWindow.show();  //��ʾBuffer����
}

void CUGGIS::on_actionSHPtoGRID_triggered()
{
	SHPtoGRIDWindow.show();//��ʾSHPtoGRIDW����
}

void CUGGIS::on_actionRectangle_triggered()
{
	// ��� m_pSelectTool Ϊ nullptr����ʼ����
	if (m_pSelectTool == nullptr) {
		m_pSelectTool = new QgsMapToolSelect(m_mapCanvas);  // ��ʼ�� QgsMapToolSelect
	}

	// �洢Ҫ����ѡ���ͼ��
	QgsVectorLayer* pSelectLayer = nullptr;
	if (m_mapCanvas->layerCount() > 0) {
		pSelectLayer = (QgsVectorLayer*)m_mapCanvas->layer(0);  // ��ȡ��һ��ͼ��
		m_pSelectTool->SetSelectLayer(pSelectLayer);  // ����ͼ��
	}
	m_pSelectTool->SetEnable(true);  // ����ѡ�񹤾�
	m_mapCanvas->setMapTool(m_pSelectTool);  // ���õ�ǰ����Ϊѡ�񹤾�
	m_mapCanvas->refresh();  // ˢ�»���
}

void CUGGIS::on_actionCircle_triggered()
{
	// ��� m_pCircleCut Ϊ nullptr����ʼ����
	if (m_pCircleCut == nullptr) {
		m_pCircleCut = new CircleCut(m_mapCanvas);  // ��ʼ�� QgsMapToolSelect
	}

	// �洢Ҫ����ѡ���ͼ��
	QgsVectorLayer* pSelectLayer2 = nullptr;
	if (m_mapCanvas->layerCount() > 0) {
		pSelectLayer2 = (QgsVectorLayer*)m_mapCanvas->layer(0);  // ��ȡ��һ��ͼ��
		m_pCircleCut->SetSelectLayer(pSelectLayer2);  // ����ͼ��
	}
	m_pCircleCut->SetEnable(true);  // ����ѡ�񹤾�
	m_mapCanvas->setMapTool(m_pCircleCut);  // ���õ�ǰ����Ϊѡ�񹤾�
	m_mapCanvas->refresh();  // ˢ�»���
}

void CUGGIS::on_actionRandom_triggered()
{
	// ��� m_pPolygonCut Ϊ nullptr����ʼ����
	if (m_pPolygonCut == nullptr) {
		m_pPolygonCut = new PolygonCut(m_mapCanvas);  // ��ʼ�� QgsMapToolSelect
	}

	// �洢Ҫ����ѡ���ͼ��
	QgsVectorLayer* pSelectLayer2 = nullptr;
	if (m_mapCanvas->layerCount() > 0) {
		pSelectLayer2 = (QgsVectorLayer*)m_mapCanvas->layer(0);  // ��ȡ��һ��ͼ��
		m_pPolygonCut->SetSelectLayer(pSelectLayer2);  // ����ͼ��
	}
	m_pPolygonCut->SetEnable(true);  // ����ѡ�񹤾�
	m_mapCanvas->setMapTool(m_pPolygonCut);  // ���õ�ǰ����Ϊѡ�񹤾�
	m_mapCanvas->refresh();  // ˢ�»���
}
