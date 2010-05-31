#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <QProgressDialog>
#include <QPointer>
#include <QDebug>

namespace BioSig_
{

class ProgressBar
{
public:
    static ProgressBar& instance ()
    {
        static ProgressBar instance_;
        return instance_;
    }

    void initAndShow (int max_value, QString const& title)
    {
        value_ = 0;
        progress_dialog_ = new QProgressDialog;
        progress_dialog_->setModal (true);
        progress_dialog_->setMinimumDuration (2000);
        progress_dialog_->setWindowTitle (title);
        progress_dialog_->setMinimum (value_);
        progress_dialog_->setMaximum (max_value);
        progress_dialog_->setValue (value_);
        qDebug () << title << ": max_value = " << max_value;
    }

    void close ()
    {
        delete progress_dialog_;
    }

    bool increaseValue (int step, QString const& description)
    {
        if (progress_dialog_.isNull())
            return true;

        if (progress_dialog_->wasCanceled())
            return false;
        progress_dialog_->setLabelText (description);
        value_ += step;
        progress_dialog_->setValue (value_);
        return true;
    }

private:
    QPointer<QProgressDialog> progress_dialog_;
    int value_;
};

}

#endif // PROGRESS_BAR_H
