#include "resampling.h"
#include "ui_resampling.h"

Resampling::Resampling(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Resampling)
{
    ui->setupUi(this);
}

Resampling::Resampling(int nativeSrate, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Resampling)
{
    ui->setupUi(this);
    QString text = "The native sample rate of the current file is ";
    text += QString::number(nativeSrate);
    text += "Hz. <br>Would you like to resample it?";
    ui->label->setText(text);
    ui->spinBox->setValue(nativeSrate);
}


Resampling::~Resampling()
{
    delete ui;
}

int Resampling::getUserSrate()
{
    return userSrate;
}

void Resampling::on_buttonBox_accepted()
{
    userSrate = ui->spinBox->value();
}
