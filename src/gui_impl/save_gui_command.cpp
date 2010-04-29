#include "save_gui_command.h"
#include "../file_handling/file_signal_writer_factory.h"
#include "../application_context.h"

namespace BioSig_
{

QString const SaveGuiCommand::SAVE_AS_ = "Save as...";
QString const SaveGuiCommand::SAVE_ = "Save";

QStringList const SaveGuiCommand::ACTIONS_ = QStringList() <<
                                             SaveGuiCommand::SAVE_AS_ <<
                                             SaveGuiCommand::SAVE_;



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
    resetActionTriggerSlot (SAVE_, SLOT(save()));
}


//-----------------------------------------------------------------------------
void SaveGuiCommand::saveAs ()
{
    FileSignalWriter* writer = FileSignalWriterFactory::getInstance()->getElement(".gdf");

    QSharedPointer<ChannelManager> channel_mgr = ApplicationContext::getInstance()->getCurrentFileContext()->getChannelManager();
    QSharedPointer<EventManager> event_mgr = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();

    writer->newSave (channel_mgr, event_mgr, "/home/ce/test.gdf");
}

//-----------------------------------------------------------------------------
void SaveGuiCommand::save ()
{
    QString file_path = ApplicationContext::getInstance()->getCurrentFileContext()->getFilePathAndName();
    FileSignalWriter* writer = FileSignalWriterFactory::getInstance()->getElement(file_path.right(4));
    QSharedPointer<EventManager> event_mgr = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    writer->saveEvents(event_mgr, file_path);
}

}
