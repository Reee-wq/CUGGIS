#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CUGGIS.h"

class CUGGIS : public QMainWindow
{
    Q_OBJECT

public:
    CUGGIS(QWidget *parent = nullptr);
    ~CUGGIS();

private:
    Ui::CUGGISClass ui;
};
