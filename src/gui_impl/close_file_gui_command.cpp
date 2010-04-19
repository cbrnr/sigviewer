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
    ApplicationContext::getInstance()->removeCurrentFileContext ();
    //    QString extensions = FileSignalReaderFactory::getInstance()->getExtensions();
//    QSettings settings ("SigViewer");
//    QString open_path = settings.value("file_open_path", ".").toString();
//    if (!open_path.length())
//        open_path = QDir::homePath ();
//    QString file_path = showOpenDialog (open_path, extensions);
//
//    if (file_path.isEmpty())
//        return;
//
//    openFile (file_path);
}


} // namespace BioSig_
