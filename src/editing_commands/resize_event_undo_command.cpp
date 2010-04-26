#include "resize_event_undo_command.h"
#include "../file_handling/event_manager.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ResizeEventUndoCommand::ResizeEventUndoCommand (QSharedPointer<EventManager> event_manager,
                                                EventID id,
                                                uint32 new_start_position,
                                                uint32 new_duration)
: event_manager_ (event_manager),
  event_id_ (id),
  new_start_position_ (new_start_position),
  new_duration_ (new_duration)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ResizeEventUndoCommand::undo ()
{
    QSharedPointer<SignalEvent> event = event_manager_->getAndLockEventForEditing (event_id_);
    event->setDuration (old_duration_);
    event->setPosition (old_start_position_);
    event_manager_->updateAndUnlockEvent (event->getId());
}

//-----------------------------------------------------------------------------
void ResizeEventUndoCommand::redo ()
{
    QSharedPointer<SignalEvent> event = event_manager_->getAndLockEventForEditing (event_id_);
    old_duration_ = event->getDuration();
    old_start_position_ = event->getPosition();
    event->setDuration (new_duration_);
    event->setPosition (new_start_position_);
    event_manager_->updateAndUnlockEvent (event->getId());
}



}
