#include "Layering.h"
//ȷ��utf-8����
#pragma execution_character_set("utf-8")  // ȷ��utf-8���룬����ȫ���ַ���������


Layering::Layering(QWidget* parent, QgsMapLayer* layer)
    : QMainWindow(parent), m_curMapLayer(layer)  // �ڹ��캯���г�ʼ�� m_curMapLayer
{
    ui.setupUi(this);

    QgsRasterLayer* rasterLayer = qobject_cast<QgsRasterLayer*>(m_curMapLayer);
    if (rasterLayer)
    {
        

        mStyle = StyleManager::getStyle(); //��ȡ��ʽ
        // ȷ�� mModel ����ʼ��
        mModel = new QgsStyleProxyModel(mStyle);

        // ����ͼ���С�������С 
        double iconSize = Qgis::UI_SCALE_FACTOR * fontMetrics().maxWidth() * 10; // ͼ���С
        ui.comboBox->setIconSize(QSize(static_cast<int>(iconSize * 0.3), static_cast<int>(iconSize * 0.3)));
        mModel->addDesiredIconSize(ui.comboBox->iconSize()); // ����ͼ���С
        // ����ģ�͵���ͼ
        ui.comboBox->setModel(mModel);


        mModel->setEntityFilterEnabled(true);
        mModel->setSymbolTypeFilterEnabled(false);
        mModel->setEntityFilter(QgsStyle::ColorrampEntity); // ����ɸѡ��Ϊ��ɫ����

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
        QgsRasterShader* shader = new QgsRasterShader();  // ����դ����ɫ��
        QgsColorRampShader* colorRampShader = new QgsColorRampShader();  // ������ɫ������ɫ��
        colorRampShader->setSourceColorRamp(colorRamp);  // ������ɫ����
        QList<QgsColorRampShader::ColorRampItem> colorRampItems;  // ��ɫ�����б�
        for (int i = 0; i < numClasses; ++i) {
            QgsColorRampShader::ColorRampItem item;
            item.value = breaks[i];
            item.color = colors[i];
            colorRampItems.append(item);
        }
        colorRampShader->setColorRampItemList(colorRampItems);  // ������ɫ�����б�
        shader->setRasterShaderFunction(colorRampShader);  // ������ɫ��

        QgsSingleBandPseudoColorRenderer* renderer = new QgsSingleBandPseudoColorRenderer(rasterLayer->dataProvider(), 1, shader);
        rasterLayer->setRenderer(renderer);
        rasterLayer->triggerRepaint();

       this->close();
    }else{
        QMessageBox::warning(this, "��ʾ", "���Ƚ��зֶμ��㣡");
    }
}
void Layering::on_buttonCalculate_click() {
    if (!isCalculated) {
        int intValue = ui.spinBox->value();  // ��ȡ�ֶ���
        if (intValue < 2) {
            QMessageBox::warning(this, "��ʾ", "�ֶ�������Ϊ2��");
            return;
        }

        numClasses = intValue;  // ���÷ֶ���
        isCalculated = true;
        QgsRasterLayer* rasterLayer = qobject_cast<QgsRasterLayer*>(m_curMapLayer);
        QgsRasterDataProvider* dataProvider = rasterLayer->dataProvider();

        // ��ȡ����ͳ����Ϣ
        QgsRasterBandStats stats = dataProvider->bandStatistics(1, QgsRasterBandStats::All);

        double minValue = stats.minimumValue;  // ��ȡ��Сֵ
        double maxValue = stats.maximumValue;  // ��ȡ���ֵ
        double range = maxValue - minValue;  // ���㷶Χ


        for (int i = 0; i < numClasses; ++i) {
            double breakValue = minValue + range * i / numClasses;
            breaks.push_back(breakValue);
        }


        QString symbolName = ui.comboBox->currentText();
        // ��ȡ��ɫ����
        colorRamp = mStyle->colorRamp(symbolName);

        for (int i = 0; i < numClasses; ++i) {
            QColor color = colorRamp->color(i * 1.0 / numClasses);


            colors.push_back(color);
        }

        // ����ģ��
        QStandardItemModel* model = new QStandardItemModel(this);

        // ����ɫ�ͷֶ�ֵ��ӵ�ģ��
        for (int i = 0; i < numClasses; ++i) {
            QStandardItem* item = new QStandardItem();

            // ���������ñ���ɫ
            item->setBackground(colors[i]);

            // �����ı����ֶ�ֵ��
            item->setText(QString("�ֶ�ֵ: %1, ��ɫ: %2")
                .arg(breaks[i])
                .arg(colors[i].name()));  // ��ȡ��ɫ��HEXֵ

            // ������ӵ�ģ����
            model->appendRow(item);
        }

        // ��ģ�����õ�QListView��
        ui.listView->setModel(model);
    }
    else {
        QMessageBox::warning(this, "��ʾ", "�Ѽ���,����ȷ��");
    }
    
}

