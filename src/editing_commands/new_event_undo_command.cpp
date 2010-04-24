#include "new_event_undo_command.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
NewEventUndoCommand::NewEventUndoCommand (QSharedPointer<EventManager> event_manager,
                                          QSharedPointer<SignalEvent const> signal_event,
                                          float scene_to_signal_factor)
 : event_manager_ (event_manager),
   raw_signal_event_ (new SignalEvent (*signal_event))
{
    raw_signal_event_->setPosition (scene_to_signal_factor * raw_signal_event_->getPosition());
    raw_signal_event_->setDuration (scene_to_signal_factor * raw_signal_event_->getDuration());
}

//-----------------------------------------------------------------------------
NewEventUndoCommand::~NewEventUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void NewEventUndoCommand::undo ()
{
    event_manager_->removeEvent (created_signal_event_->getId ());
}

//-----------------------------------------------------------------------------
void NewEventUndoCommand::redo ()
{
    EventID id = UNDEFINED_EVENT_ID;
    if (!created_signal_event_.isNull())
        id = created_signal_event_->getId ();

    created_signal_event_ = event_manager_->createEvent (
            raw_signal_event_->getChannel(),
            raw_signal_event_->getPosition(),
            raw_signal_event_->getDuration(),
            raw_signal_event_->getType(),
            id);
}

}
