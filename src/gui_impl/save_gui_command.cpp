#include "save_gui_command.h"
#include "../file_handling/file_signal_writer_factory.h"
#include "../application_context.h"

namespace BioSig_
{

QString const SaveGuiCommand::SAVE_AS_ = "Save as...";
QStringList const SaveGuiCommand::ACTIONS_ = QStringList() <<
                                             SaveGuiCommand::SAVE_AS_;



//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator SaveGuiCommand::registrator_ ("Saving",
                                                          QSharedPointer<SaveGuiCommand> (new SaveGuiCommand));


//-----------------------------------------------------------------------------
SaveGuiCommand::SaveGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{

}

//-----------------------------------------------------------------------------
void SaveGuiCommand::init ()
{
    resetActionTriggerSlot (SAVE_AS_, SLOT(saveAs()));
}


//-----------------------------------------------------------------------------
void SaveGuiCommand::saveAs ()
{
    FileSignalWriter* writer = FileSignalWriterFactory::getInstance()->getElement(".gdf");

    QSharedPointer<ChannelManager> channel_mgr = ApplicationContext::getInstance()->getCurrentFileContext()->getChannelManager();
    QSharedPointer<EventManager> event_mgr (0);


    writer->newSave (channel_mgr, event_mgr, "/home/ce/test.gdf");
}

}
