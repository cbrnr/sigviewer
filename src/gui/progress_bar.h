#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include "application_context.h"

#include <QProgressDialog>
#include <QPointer>
#include <QDebug>

namespace SigViewer_
{

class ProgressBar
{
public:
    static ProgressBar& instance ()
    {
        static ProgressBar instance_;
        return instance_;
    }

    void initAndShow (int max_value, QString const& title,
                      QSharedPointer<ApplicationContext const> app_context)
    {
        non_gui_mode_ = app_context->modeActivated (APPLICATION_NON_GUI_MODE);
        value_ = 0;
        max_value_ = max_value;
        if (non_gui_mode_)
            return;
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
        value_ += step;
        if (non_gui_mode_)
            return true;
        if (progress_dialog_.isNull())
            return true;

        if (progress_dialog_->wasCanceled())
            return false;
        progress_dialog_->setLabelText (description);
        progress_dialog_->setValue (value_);
        return true;
    }

private:
    QPointer<QProgressDialog> progress_dialog_;
    bool non_gui_mode_;
    int max_value_;
    int value_;
};

}

#endif // PROGRESS_BAR_H
