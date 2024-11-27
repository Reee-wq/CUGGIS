#include "LabelControl.h"
#include "ui_LabelControl.h"
#include <QDebug>

LabelControl::LabelControl(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LabelControlClass())
{
	ui->setupUi(this);
    QStringList ll;
    ll << QString::fromLocal8Bit("显示") << QString::fromLocal8Bit("不显示");
    ui->comboBox_show->addItems(ll);
    ui->comboBox_show->setCurrentIndex(1);
    m_bshowLabels = false;
    m_strShowName = "";

    setWindowTitle(QString::fromLocal8Bit("标签设置"));

    QPalette pe = this->palette();
    pe.setColor(QPalette::Background, Qt::white);
    setPalette(pe);
   
}

LabelControl::~LabelControl()
{
	delete ui;
}

void LabelControl::SetItems(QStringList items)
{
    ui->comboBox_items->clear();
    ui->comboBox_items->addItems(items);
}

void LabelControl::SetInitInfo(bool bshow, QString name)
{
    if (bshow)
        ui->comboBox_show->setCurrentIndex(0);
    else
        ui->comboBox_show->setCurrentIndex(1);

    if (!name.isEmpty())
        ui->comboBox_items->setCurrentText(name);
   
}

QString LabelControl::GetSelectedItem()
{
    return ui->comboBox_items->currentText();
}

bool LabelControl::isShowLabel()
{
	return true;
}
//OK
void LabelControl::on_btnOk_clicked()
{
    qDebug() << "Button clicked!";
    int ind = ui->comboBox_show->currentIndex();
    bool sh = false;
    if (ind == 0) sh = true;

    QString name = ui->comboBox_items->currentText();

    if (sh != m_bshowLabels || name != m_strShowName)
    {
        m_bshowLabels = sh;
        m_strShowName = name;
        emit sig_labelctrlBtnClicked(1, m_bshowLabels, m_strShowName);
    }
    else
        emit sig_labelctrlBtnClicked(0, m_bshowLabels, m_strShowName);  //Do nothing}

}


void LabelControl::on_btnCancle_clicked()
{
    emit sig_labelctrlBtnClicked(0, m_bshowLabels, m_strShowName);
}
