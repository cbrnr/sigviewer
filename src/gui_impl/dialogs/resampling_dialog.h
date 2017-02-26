// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef RESAMPLING_DIALOG_H
#define RESAMPLING_DIALOG_H

#include <QDialog>

namespace Ui {
class ResamplingDialog;
}

namespace sigviewer {

class ResamplingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResamplingDialog(QWidget *parent = 0);
    ResamplingDialog(int nativeSrate, int highestSampleRate, QWidget *parent = 0);

    ~ResamplingDialog();

    int getUserSrate() {return userSrate;}

private slots:

    void on_resample_button_clicked();

    void on_cancel_button_clicked();

private:
    Ui::ResamplingDialog *ui;
    int userSrate = 0;
};

}
#endif // RESAMPLING_DIALOG_H
