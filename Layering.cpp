#include "Layering.h"
//确保utf-8编码
#pragma execution_character_set("utf-8")  // 确保utf-8编码，避免全角字符出现乱码


Layering::Layering(QWidget* parent, QgsMapLayer* layer)
    : QMainWindow(parent), m_curMapLayer(layer)  // 在构造函数中初始化 m_curMapLayer
{
    ui.setupUi(this);

    QgsRasterLayer* rasterLayer = qobject_cast<QgsRasterLayer*>(m_curMapLayer);
    if (rasterLayer)
    {
        

        mStyle = StyleManager::getStyle(); //获取样式
        // 确保 mModel 被初始化
        mModel = new QgsStyleProxyModel(mStyle);

        // 配置图标大小和网格大小 
        double iconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().maxWidth() * 10; // 图标大小
        ui.comboBox->setIconSize(QSize(static_cast<int>(iconSize * 0.3), static_cast<int>(iconSize * 0.3)));
        mModel->addDesiredIconSize(ui.comboBox->iconSize()); // 设置图标大小
        // 设置模型到视图
        ui.comboBox->setModel(mModel);


        mModel->setEntityFilterEnabled(true);
        mModel->setSymbolTypeFilterEnabled(false);
        mModel->setEntityFilter(QgsStyle::ColorrampEntity); // 设置筛选器为颜色渐变

        connect(ui.buttonConfirm, &QPushButton::clicked, this, &Layering::on_buttonConfirm_click);
        connect(ui.buttonCalculate, &QPushButton::clicked, this, &Layering::on_buttonCalculate_click);
        connect(ui.buttonCancel, &QPushButton::clicked, this, &Layering::close);
    }
}

Layering::~Layering()
{}

void  Layering::on_buttonConfirm_click() {
    if (isCalculated) {
        QgsRasterLayer* rasterLayer = qobject_cast<QgsRasterLayer*>(m_curMapLayer);
        QgsRasterShader* shader = new QgsRasterShader();  // 创建栅格着色器
        QgsColorRampShader* colorRampShader = new QgsColorRampShader();  // 创建颜色渐变着色器
        colorRampShader->setSourceColorRamp(colorRamp);  // 设置颜色渐变
        QList<QgsColorRampShader::ColorRampItem> colorRampItems;  // 颜色渐变列表
        for (int i = 0; i < numClasses; ++i) {
            QgsColorRampShader::ColorRampItem item;
            item.value = breaks[i];
            item.color = colors[i];
            colorRampItems.append(item);
        }
        colorRampShader->setColorRampItemList(colorRampItems);  // 设置颜色渐变列表
        shader->setRasterShaderFunction(colorRampShader);  // 设置着色器

        QgsSingleBandPseudoColorRenderer* renderer = new QgsSingleBandPseudoColorRenderer(rasterLayer->dataProvider(), 1, shader);
        rasterLayer->setRenderer(renderer);
        rasterLayer->triggerRepaint();

       this->close();
    }else{
        QMessageBox::warning(this, "提示", "请先进行分段计算！");
    }
}
void Layering::on_buttonCalculate_click() {
    if (!isCalculated) {
        int intValue = ui.spinBox->value();  // 获取分段数
        if (intValue < 2) {
            QMessageBox::warning(this, "提示", "分段数至少为2！");
            return;
        }

        numClasses = intValue;  // 设置分段数
        isCalculated = true;
        QgsRasterLayer* rasterLayer = qobject_cast<QgsRasterLayer*>(m_curMapLayer);
        QgsRasterDataProvider* dataProvider = rasterLayer->dataProvider();

        // 获取波段统计信息
        QgsRasterBandStats stats = dataProvider->bandStatistics(1, QgsRasterBandStats::All);

        double minValue = stats.minimumValue;  // 获取最小值
        double maxValue = stats.maximumValue;  // 获取最大值
        double range = maxValue - minValue;  // 计算范围


        for (int i = 0; i < numClasses; ++i) {
            double breakValue = minValue + range * i / numClasses;
            breaks.push_back(breakValue);
        }


        QString symbolName = ui.comboBox->currentText();
        // 获取颜色渐变
        colorRamp = mStyle->colorRamp(symbolName);

        for (int i = 0; i < numClasses; ++i) {
            QColor color = colorRamp->color(i * 1.0 / numClasses);


            colors.push_back(color);
        }

        // 创建模型
        QStandardItemModel* model = new QStandardItemModel(this);

        // 将颜色和分段值添加到模型
        for (int i = 0; i < numClasses; ++i) {
            QStandardItem* item = new QStandardItem();

            // 创建并设置背景色
            item->setBackground(colors[i]);

            // 创建文本（分段值）
            item->setText(QString("分段值: %1, 颜色: %2")
                .arg(breaks[i])
                .arg(colors[i].name()));  // 获取颜色的HEX值

            // 将项添加到模型中
            model->appendRow(item);
        }

        // 将模型设置到QListView中
        ui.listView->setModel(model);
    }
    else {
        QMessageBox::warning(this, "提示", "已计算,请先确认");
    }
    
}

