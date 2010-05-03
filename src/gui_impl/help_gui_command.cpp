#include "help_gui_command.h"

#include <QMessageBox>

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const HelpGuiCommand::ABOUT_ = "About";
QStringList const HelpGuiCommand::ACTIONS_ = QStringList() <<
                                             HelpGuiCommand::ABOUT_;

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
}

//-----------------------------------------------------------------------------
void HelpGuiCommand::showAboutDialog ()
{
    QMessageBox about_message_box (tr("About SigViewer"),
                                   tr("<b><nobr>SigViewer Version 0.3.1</nobr></b><br>\
                                       <br>\
                                       Christoph Eibel, Thomas Brunner, Clemens Brunner, Alois Schl&ouml;gl, Oliver Terbu<br>\
                                       <br>\
                                       <nobr>Graz University of Technology</nobr>"),
                                   QMessageBox::NoIcon,
                                   QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    about_message_box.setIconPixmap (QPixmap (":/images/sigviewer128.png"));
    about_message_box.exec ();
}


} // namespace BioSig_
