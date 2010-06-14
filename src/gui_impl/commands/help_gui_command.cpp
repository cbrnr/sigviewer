#include "help_gui_command.h"
#include "tests/tests_dialog.h"

#include <QMessageBox>

namespace BioSig_
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
    QMessageBox about_message_box (tr("About SigViewer"),
                                   tr("<b><nobr>SigViewer Version 0.4.0</nobr></b><br>\
                                       <br>\
                                       Christoph Eibel, Thomas Brunner, Clemens Brunner, Alois Schl&ouml;gl, Oliver Terbu<br>\
                                       <br>\
                                       <nobr>Graz University of Technology</nobr>"),
                                   QMessageBox::NoIcon,
                                   QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    about_message_box.setIconPixmap (QPixmap (":/images/sigviewer128.png"));
    about_message_box.exec ();
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::runTests ()
{
    TestsDialog tests_dialog;
    tests_dialog.exec ();
}

} // namespace BioSig_
