// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef HOTKEY_DIALOG_H
#define HOTKEY_DIALOG_H

#include <QDialog>

namespace Ui {
class HotkeyDialog;
}

class HotkeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HotkeyDialog(QWidget *parent = 0);
    ~HotkeyDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::HotkeyDialog *ui;
};

#endif // HOTKEY_DIALOG_H
