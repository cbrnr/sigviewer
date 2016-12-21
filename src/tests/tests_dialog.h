// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef TESTS_DIALOG_H
#define TESTS_DIALOG_H

#include "ui_tests_dialog.h"

#include "test.h"

#include <QDialog>
#include <QList>
#include <QSharedPointer>

namespace sigviewer
{

//-----------------------------------------------------------------------------
/// TestsDialog
/// executes tests and displays results
class TestsDialog : public QDialog
{
    Q_OBJECT
public:
    TestsDialog ();

private slots:
    void on_start_button__clicked ();
    void on_open_dummy__clicked ();

private:
    void runTests ();

    Ui::TestDialog ui_;
};

}

#endif // TESTS_DIALOG_H
