#include "change_channel_undo_command.h"
#include "file_handling/event_manager.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
ChangeChannelUndoCommand::ChangeChannelUndoCommand (QSharedPointer<EventManager> event_manager,
                                                    EventID event_id,
                                                    ChannelID new_channel)
 : event_manager_ (event_manager),
   event_id_ (event_id),
   new_channel_ (new_channel)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
ChangeChannelUndoCommand::~ChangeChannelUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ChangeChannelUndoCommand::undo ()
{
    QSharedPointer<SignalEvent> signal_event_ = event_manager_->getAndLockEventForEditing (event_id_);
    signal_event_->setChannel (old_channel_);
    event_manager_->updateAndUnlockEvent (signal_event_->getId());
}

//-----------------------------------------------------------------------------
void ChangeChannelUndoCommand::redo ()
{
    QSharedPointer<SignalEvent> signal_event_ = event_manager_->getAndLockEventForEditing (event_id_);
    old_channel_ = signal_event_->getChannel();
    signal_event_->setChannel (new_channel_);
    event_manager_->updateAndUnlockEvent (signal_event_->getId());
}

}
