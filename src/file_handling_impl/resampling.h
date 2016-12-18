#ifndef RESAMPLING_H
#define RESAMPLING_H

#include <QDialog>

namespace Ui {
class Resampling;
}

namespace SigViewer_ {

class Resampling : public QDialog
{
    Q_OBJECT

public:
    explicit Resampling(QWidget *parent = 0);
    Resampling(int nativeSrate, int highestSampleRate, QWidget *parent = 0);

    ~Resampling();

    int getUserSrate();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Resampling *ui;
    int userSrate = 0;
};

}

#endif // RESAMPLING_H