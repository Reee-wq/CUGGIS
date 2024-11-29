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

// 注册 QgsSymbol 类型
Q_DECLARE_METATYPE(QgsSymbol*)

SymbolExport::SymbolExport(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    mStyle = StyleManager::getStyle(); //获取样式
    // 确保 mModel 被初始化
    mModel = new QgsStyleProxyModel(mStyle);

    // 配置图标大小和网格大小 
    double iconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().maxWidth() * 10; // 图标大小
    ui.listView->setIconSize(QSize(static_cast<int>(iconSize * 0.3), static_cast<int>(iconSize * 0.3)));
    ui.listView->setGridSize(QSize(static_cast<int>(iconSize * 0.35), static_cast<int>(iconSize * 0.45)));
    mModel->addDesiredIconSize(ui.listView->iconSize()); // 设置图标大小

    // 设置模型到视图
    ui.listView->setModel(mModel);
    ui.listView->setSelectionBehavior(QAbstractItemView::SelectItems);  // 单选
    ui.listView->setSelectionMode(QAbstractItemView::ExtendedSelection);  // 多选
    ui.listView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 不允许编辑
    ui.listView->setWordWrap(true);  // 自动换行

    // 禁用符号类型过滤器
    mModel->setSymbolTypeFilterEnabled(false);  // 禁用符号类型过滤器

    // 设置多个符号类型
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
    // 获取当前选中的符号项
    QModelIndexList selectedIndexes = ui.listView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "没有选择符号", "请至少选择一个符号进行导出！");
        return;
    }

    // 获取第一个选中的索引
    QModelIndex index = selectedIndexes.first();
    QVariant data = mModel->data(index);
    QString symbolName = data.toString();

    // 获取符号对象
    QgsSymbol* symbol = mStyle->symbol(symbolName);

    if (!symbol) {
        QMessageBox::warning(this, "符号错误", "无法获取符号！");
        return;
    }

    // 打开文件保存对话框
    QString fileName = QFileDialog::getSaveFileName(this, "保存为 SLD 文件", "", "SLD Files (*.sld)");
    if (fileName.isEmpty()) {
        return;
    }

    // 手动拼接 SLD XML
    QString wellKnownName = "circle"; // 示例符号形状
    QString fillColor = "#8f2543";   // 示例填充颜色
    QString strokeColor = "#232323"; // 示例边框颜色
    QString strokeWidth = "0.5";     // 示例边框宽度
    QString size = "20";              // 示例大小


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

    // 将 SLD 内容写入文件
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "文件错误", "无法写入文件！");
        return;
    }

    QTextStream out(&file);
    out << sldContent;
    file.close();

    QMessageBox::information(this, "导出成功", "符号已导出到 " + fileName + " 文件！");



}
