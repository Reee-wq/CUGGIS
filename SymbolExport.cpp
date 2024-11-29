#pragma execution_character_set("utf-8")
#include "SymbolExport.h"
#include "ui_SymbolExport.h"
#include<QgsSymbolLayerUtils.h>
#include <qgis.h>
#include<QDomDocument>
#include<QXml.h>
#include<QFile>
#include<string>
#include<QMessageBox>
#include<QgsStyle.h>


#include "qgsstyle.h"

// ע�� QgsSymbol ����
Q_DECLARE_METATYPE(QgsSymbol*)

SymbolExport::SymbolExport(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    mStyle = StyleManager::getStyle(); //��ȡ��ʽ
    // ȷ�� mModel ����ʼ��
    mModel = new QgsStyleProxyModel(mStyle);

    // ����ͼ���С�������С 
    double iconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().maxWidth() * 10; // ͼ���С
    ui.listView->setIconSize(QSize(static_cast<int>(iconSize * 0.3), static_cast<int>(iconSize * 0.3)));
    ui.listView->setGridSize(QSize(static_cast<int>(iconSize * 0.35), static_cast<int>(iconSize * 0.45)));
    mModel->addDesiredIconSize(ui.listView->iconSize()); // ����ͼ���С

    // ����ģ�͵���ͼ
    ui.listView->setModel(mModel);
    ui.listView->setSelectionBehavior(QAbstractItemView::SelectItems);  // ��ѡ
    ui.listView->setSelectionMode(QAbstractItemView::ExtendedSelection);  // ��ѡ
    ui.listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // ������༭
    ui.listView->setWordWrap(true);  // �Զ�����

    // ���÷������͹�����
    mModel->setSymbolTypeFilterEnabled(false);  // ���÷������͹�����

    // ���ö����������
    QVector<QgsSymbol::SymbolType> allSymbolTypes = {
        QgsSymbol::SymbolType::Marker,
        QgsSymbol::SymbolType::Line,
        QgsSymbol::SymbolType::Fill
    };

    for (const auto& symbolType : allSymbolTypes) {
        mModel->setSymbolType(symbolType);
    }

    connect(ui.buttonExport, &QPushButton::clicked, this, &SymbolExport::onbuttonExportClicked);
    connect(ui.buttonExit, &QPushButton::clicked, this, &SymbolExport::hide);
}

SymbolExport::~SymbolExport()
{}

void SymbolExport::onbuttonExportClicked() {
    // ��ȡ��ǰѡ�еķ�����
    QModelIndexList selectedIndexes = ui.listView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "û��ѡ�����", "������ѡ��һ�����Ž��е�����");
        return;
    }

    // ��ȡ��һ��ѡ�е�����
    QModelIndex index = selectedIndexes.first();
    QVariant data = mModel->data(index);
    QString symbolName = data.toString();

    // ��ȡ���Ŷ���
    QgsSymbol* symbol = mStyle->symbol(symbolName);

    if (!symbol) {
        QMessageBox::warning(this, "���Ŵ���", "�޷���ȡ���ţ�");
        return;
    }

    // ���ļ�����Ի���
    QString fileName = QFileDialog::getSaveFileName(this, "����Ϊ SLD �ļ�", "", "SLD Files (*.sld)");
    if (fileName.isEmpty()) {
        return;
    }

    // �ֶ�ƴ�� SLD XML
    QString wellKnownName = "circle"; // ʾ��������״
    QString fillColor = "#8f2543";   // ʾ�������ɫ
    QString strokeColor = "#232323"; // ʾ���߿���ɫ
    QString strokeWidth = "0.5";     // ʾ���߿���
    QString size = "20";              // ʾ����С


    QString sldContent;
    sldContent += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    sldContent += "<StyledLayerDescriptor xmlns=\"http://www.opengis.net/sld\" ";
    sldContent += "xmlns:ogc=\"http://www.opengis.net/ogc\" ";
    sldContent += "xmlns:xlink=\"http://www.w3.org/1999/xlink\" ";
    sldContent += "xsi:schemaLocation=\"http://www.opengis.net/sld http://schemas.opengis.net/sld/1.1.0/StyledLayerDescriptor.xsd\" ";
    sldContent += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
    sldContent += "xmlns:se=\"http://www.opengis.net/se\" version=\"1.1.0\">\n";
    sldContent += "  <NamedLayer>\n";
    sldContent += "    <se:Name>" + symbolName + "</se:Name>\n";
    sldContent += "    <UserStyle>\n";
    sldContent += "      <se:Name>" + symbolName + "</se:Name>\n";
    sldContent += "      <se:FeatureTypeStyle>\n";
    sldContent += "        <se:Rule>\n";
    sldContent += "          <se:Name>Single symbol</se:Name>\n";
    sldContent += "          <se:PointSymbolizer>\n";
    sldContent += "            <se:Graphic>\n";
    sldContent += "              <se:Mark>\n";
    sldContent += "                <se:WellKnownName>" + wellKnownName + "</se:WellKnownName>\n";
    sldContent += "                <se:Fill>\n";
    sldContent += "                  <se:SvgParameter name=\"fill\">" + fillColor + "</se:SvgParameter>\n";
    sldContent += "                </se:Fill>\n";
    sldContent += "                <se:Stroke>\n";
    sldContent += "                  <se:SvgParameter name=\"stroke\">" + strokeColor + "</se:SvgParameter>\n";
    sldContent += "                  <se:SvgParameter name=\"stroke-width\">" + strokeWidth + "</se:SvgParameter>\n";
    sldContent += "                </se:Stroke>\n";
    sldContent += "              </se:Mark>\n";
    sldContent += "              <se:Size>" + size + "</se:Size>\n";
    sldContent += "            </se:Graphic>\n";
    sldContent += "          </se:PointSymbolizer>\n";
    sldContent += "        </se:Rule>\n";
    sldContent += "      </se:FeatureTypeStyle>\n";
    sldContent += "    </UserStyle>\n";
    sldContent += "  </NamedLayer>\n";
    sldContent += "</StyledLayerDescriptor>\n";

    // �� SLD ����д���ļ�
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "�ļ�����", "�޷�д���ļ���");
        return;
    }

    QTextStream out(&file);
    out << sldContent;
    file.close();

    QMessageBox::information(this, "�����ɹ�", "�����ѵ����� " + fileName + " �ļ���");



}
