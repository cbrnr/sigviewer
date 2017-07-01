#ifndef HOTKEY_DIALOG_H
#define HOTKEY_DIALOG_H

#include <QDialog>

namespace Ui {
class hotkey_dialog;
}

class hotkey_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit hotkey_dialog(QWidget *parent = 0);
    ~hotkey_dialog();

private:
    Ui::hotkey_dialog *ui;
};

#endif // HOTKEY_DIALOG_H
