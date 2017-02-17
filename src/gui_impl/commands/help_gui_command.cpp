// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "help_gui_command.h"
#include "tests/tests_dialog.h"
#include "ui_about_dialog.h"

#include <QMessageBox>
#include <QFile>

namespace sigviewer
{

//-----------------------------------------------------------------------------
QString const HelpGuiCommand::ABOUT_ = "About";
QString const HelpGuiCommand::RUN_TESTS_ = "Run Tests...";
QStringList const HelpGuiCommand::ACTIONS_ = QStringList() <<
                                             HelpGuiCommand::ABOUT_ <<
                                             HelpGuiCommand::RUN_TESTS_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator HelpGuiCommand::registrator_ ("Help",
                                                          QSharedPointer<HelpGuiCommand>(new HelpGuiCommand));

//-----------------------------------------------------------------------------
HelpGuiCommand::HelpGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::init ()
{
    resetActionTriggerSlot (ABOUT_, SLOT(showAboutDialog()));
    setIcon (ABOUT_, QIcon(":/images/ic_help_outline_black_24dp.png"));
    resetActionTriggerSlot (RUN_TESTS_, SLOT(runTests()));
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::showAboutDialog ()
{
    QFile version_file (":version.txt");
    version_file.open(QIODevice::ReadOnly);
    QByteArray version_array = version_file.readLine();

    dialog_ = new QDialog(0);
    Ui::aboutDialog ui;
    ui.setupUi (&(*dialog_));
    ui.aboutLabel->setText(ui.aboutLabel->text().replace(QString("[VERSION-NUMBER]"), QString (version_array)));
    dialog_->show();
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::runTests ()
{
    TestsDialog tests_dialog;
    tests_dialog.exec ();
}

}
