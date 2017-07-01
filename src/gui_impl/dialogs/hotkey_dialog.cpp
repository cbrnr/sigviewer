#include "hotkey_dialog.h"
#include "ui_hotkey_dialog.h"

HotkeyDialog::HotkeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HotkeyDialog)
{
    ui->setupUi(this);

    resize(700, 700);
    this->setWindowTitle(tr("Hotkeys"));
    ui->tableWidget->setRowCount(6);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->horizontalHeader()->hide();
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setColumnWidth(0, width() * 0.48);
    ui->tableWidget->setColumnWidth(1, width() * 0.48);

    QString ctrl = "Ctrl";

    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg("Scale Individual Channel Up"));
    ui->tableWidget->setItem(0, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + Mousewheel Up"));
    ui->tableWidget->setItem(0, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Scale Individual Channel Down"));
    ui->tableWidget->setItem(1, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + Mousewheel Down"));
    ui->tableWidget->setItem(1, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Info Dialog"));
    ui->tableWidget->setItem(2, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + I"));
    ui->tableWidget->setItem(2, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Channel Dialog"));
    ui->tableWidget->setItem(3, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + C"));
    ui->tableWidget->setItem(3, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Event Dialog"));
    ui->tableWidget->setItem(4, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + E"));
    ui->tableWidget->setItem(4, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Zoom In horizontally"));
    ui->tableWidget->setItem(5, 0, newItem);
}

HotkeyDialog::~HotkeyDialog()
{
    delete ui;
}

void HotkeyDialog::on_pushButton_clicked()
{
    close();
}
