#include "resize_event_undo_command.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ResizeEventUndoCommand::ResizeEventUndoCommand (SignalBrowserModel& signal_browser_model, QSharedPointer<SignalEvent> event, uint32 new_start_position, uint32 new_duration)
: signal_browser_model_ (signal_browser_model),
  event_ (event),
  new_start_position_ (new_start_position),
  new_duration_ (new_duration)
{
    old_duration_ = event->getDuration();
    old_start_position_ = event->getPosition();
}

//-----------------------------------------------------------------------------
ResizeEventUndoCommand::~ResizeEventUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ResizeEventUndoCommand::undo ()
{
    event_->setDuration(old_duration_);
    event_->setPosition(old_start_position_);
    signal_browser_model_.setEventChanged(event_->getId());
}

//-----------------------------------------------------------------------------
void ResizeEventUndoCommand::redo ()
{
    event_->setDuration(new_duration_);
    event_->setPosition(new_start_position_);
    signal_browser_model_.setEventChanged(event_->getId());
}



}
