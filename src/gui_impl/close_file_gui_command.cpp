#include "close_file_gui_command.h"
#include "../application_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator CloseFileGuiCommand::registrator_ ("Close File",
                                                               QSharedPointer<CloseFileGuiCommand> (new CloseFileGuiCommand));
//-----------------------------------------------------------------------------
CloseFileGuiCommand::CloseFileGuiCommand ()
    : GuiActionCommand ("Close... (new impl)")
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
CloseFileGuiCommand::~CloseFileGuiCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void CloseFileGuiCommand::init ()
{
    //getQAction()->setShortcut (QKeySequence::Close);
    getQAction()->setIcon (QIcon(":/images/icons/fileclose.png"));
}


//-----------------------------------------------------------------------------
void CloseFileGuiCommand::trigger ()
{
    ApplicationContext::getInstance()->getMainWindowModel()->closeCurrentFileTabs ();
    ApplicationContext::getInstance()->removeCurrentFileContext ();
}


} // namespace BioSig_
