// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "help_gui_command.h"
#include "tests/tests_dialog.h"
#include "ui_about_dialog.h"

#include <QMessageBox>


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
    setIcon (ABOUT_, QIcon(":/images/baseline-help_outline-24px.svg"));
    resetActionTriggerSlot (RUN_TESTS_, SLOT(runTests()));
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::showAboutDialog ()
{
    dialog_ = new QDialog(0);
    Ui::aboutDialog ui;
    ui.setupUi (&(*dialog_));
    ui.aboutLabel->setText(ui.aboutLabel->text().replace(QString("VERSION_MAJOR"), QString::number(VERSION_MAJOR))
                                                .replace(QString("VERSION_MINOR"), QString::number(VERSION_MINOR))
                                                .replace(QString("VERSION_BUILD"), QString::number(VERSION_BUILD)));
    dialog_->setFixedSize(dialog_->minimumSizeHint());
    dialog_->show();
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::runTests ()
{
    TestsDialog tests_dialog;
    tests_dialog.exec ();
}

}
