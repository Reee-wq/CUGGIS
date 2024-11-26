#pragma once

#include <QWidget>
#include "ui_ID3andC4.h"

class ID3andC4 : public QWidget
{
	Q_OBJECT

public:
	ID3andC4(QWidget *parent = nullptr);
	QPlainTextEdit* plainTextEdit;
	~ID3andC4();

private:
	Ui::ID3andC4Class ui;
	

	
};
