#include "help_gui_command.h"
#include "tests/tests_dialog.h"
#include "ui_about_dialog.h"

#include <QMessageBox>

namespace SigViewer_
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
    resetActionTriggerSlot (RUN_TESTS_, SLOT(runTests()));
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::showAboutDialog ()
{
    QDialog dialog;
    Ui::aboutDialog ui;
    ui.setupUi (&dialog);
    dialog.exec();
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::runTests ()
{
    TestsDialog tests_dialog;
    tests_dialog.exec ();
}

} // namespace SigViewer_
