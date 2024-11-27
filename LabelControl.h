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
	void SetItems(QStringList items);  //����combox�б�����
	void SetInitInfo(bool bshow, QString name);  //���ó�ʼ��ʾ����
	QString GetSelectedItem();         //��ȡѡ�е���
	bool isShowLabel();                     //�Ƿ���ʾlabel
private slots:
	void on_btnOk_clicked();
	void on_btnCancle_clicked();
signals:
	void sig_labelctrlBtnClicked(int, bool, QString);

private:
	Ui::LabelControlClass *ui;
	bool m_bshowLabels;               //�Ƿ���ʾ�ֶ�
	QString m_strShowName;        //��ʾ���ֶ�����
};
#endif // LABELCONTROL_H