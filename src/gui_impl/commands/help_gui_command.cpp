// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "help_gui_command.h"
#include "tests/tests_dialog.h"
#include "ui_about_dialog.h"

#include <QMessageBox>
#include <QDesktopWidget>

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
    dialog_ = new QDialog(0);
    Ui::aboutDialog ui;
    ui.setupUi (&(*dialog_));
    ui.aboutLabel->setText(ui.aboutLabel->text().replace(QString("VERSION_MAJOR"), QString::number(VERSION_MAJOR))
                                                .replace(QString("VERSION_MINOR"), QString::number(VERSION_MINOR))
                                                .replace(QString("VERSION_BUILD"), QString::number(VERSION_BUILD)));

    QDesktopWidget dw;
    int x=dw.width()*0.2;
    int y=dw.height()*0.2;
    dialog_->setFixedSize(x,y);

    dialog_->show();
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::runTests ()
{
    TestsDialog tests_dialog;
    tests_dialog.exec ();
}

}
