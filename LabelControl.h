#pragma once
#ifndef LABELCONTROL_H
#define LABELCONTROL_H

#include <QWidget>
#include "ui_LabelControl.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LabelControlClass; };
QT_END_NAMESPACE

class LabelControl : public QWidget
{
	Q_OBJECT

public:
	LabelControl(QWidget *parent = nullptr);
	~LabelControl();
	void SetItems(QStringList items);  //设置combox列表内容
	void SetInitInfo(bool bshow, QString name);  //设置初始显示内容
	QString GetSelectedItem();         //获取选中的项
	bool isShowLabel();                     //是否显示label
private slots:
	void on_btnOk_clicked();
	void on_btnCancle_clicked();
signals:
	void sig_labelctrlBtnClicked(int, bool, QString);

private:
	Ui::LabelControlClass *ui;
	bool m_bshowLabels;               //是否显示字段
	QString m_strShowName;        //显示的字段名称
};
#endif // LABELCONTROL_H