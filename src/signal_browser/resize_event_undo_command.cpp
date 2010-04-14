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
  event_ (event_manager->getEventForEditing (id)),
  new_start_position_ (new_start_position),
  new_duration_ (new_duration)
{
    old_duration_ = event_->getDuration();
    old_start_position_ = event_->getPosition();
}

//-----------------------------------------------------------------------------
ResizeEventUndoCommand::~ResizeEventUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ResizeEventUndoCommand::undo ()
{
    event_->setDuration (old_duration_);
    event_->setPosition (old_start_position_);
    event_manager_->updateEvent (event_->getId());
}

//-----------------------------------------------------------------------------
void ResizeEventUndoCommand::redo ()
{
    event_->setDuration (new_duration_);
    event_->setPosition (new_start_position_);
    event_manager_->updateEvent (event_->getId());
}



}
