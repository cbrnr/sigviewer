#include "hotkey_dialog.h"
#include "ui_hotkey_dialog.h"

HotkeyDialog::HotkeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HotkeyDialog)
{
    ui->setupUi(this);

    resize(700, 700);
    this->setWindowTitle(tr("Hotkeys"));
    ui->tableWidget->setRowCount(15);
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
    newItem = new QTableWidgetItem(tr("%1").arg("Zoom In Horizontal"));
    ui->tableWidget->setItem(5, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + +"));
    ui->tableWidget->setItem(5, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Zoom Out Horizontal"));
    ui->tableWidget->setItem(6, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + -"));
    ui->tableWidget->setItem(6, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Zoom In Vertical"));
    ui->tableWidget->setItem(7, 0, newItem);
    newItem = new QTableWidgetItem(tr("Alt %1").arg(" + +"));
    ui->tableWidget->setItem(7, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Zoom Out Vertical"));
    ui->tableWidget->setItem(8, 0, newItem);
    newItem = new QTableWidgetItem(tr("Alt %1").arg(" + -"));
    ui->tableWidget->setItem(8, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Scale All"));
    ui->tableWidget->setItem(9, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + A"));
    ui->tableWidget->setItem(9, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("New Event Mode"));
    ui->tableWidget->setItem(10, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + 1"));
    ui->tableWidget->setItem(10, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Edit Event Mode"));
    ui->tableWidget->setItem(11, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + 2"));
    ui->tableWidget->setItem(11, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Scroll Mode"));
    ui->tableWidget->setItem(12, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + 3"));
    ui->tableWidget->setItem(12, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("View Options Mode"));
    ui->tableWidget->setItem(13, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + 4"));
    ui->tableWidget->setItem(13, 1, newItem);
    newItem = new QTableWidgetItem(tr("%1").arg("Open File"));
    ui->tableWidget->setItem(14, 0, newItem);
    newItem = new QTableWidgetItem(tr("%1%2").arg(ctrl).arg(" + O"));
    ui->tableWidget->setItem(14, 1, newItem);
}

HotkeyDialog::~HotkeyDialog()
{
    delete ui;
}

void HotkeyDialog::on_pushButton_clicked()
{
    close();
}
