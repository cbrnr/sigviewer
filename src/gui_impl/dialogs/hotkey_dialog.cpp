#include "hotkey_dialog.h"
#include "ui_hotkey_dialog.h"

hotkey_dialog::hotkey_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::hotkey_dialog)
{
    ui->setupUi(this);

}

hotkey_dialog::~hotkey_dialog()
{
    delete ui;
}
