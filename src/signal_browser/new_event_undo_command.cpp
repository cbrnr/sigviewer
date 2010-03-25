#include "new_event_undo_command.h"


namespace BioSig_
{

//-----------------------------------------------------------------------------
NewEventUndoCommand::NewEventUndoCommand (SignalBrowserModel& signal_browser_model,
                                          QSharedPointer<SignalEvent> signal_event,
                                          float scene_to_signal_factor)
 : signal_browser_model_ (signal_browser_model),
   signal_event_ (signal_event)
{
    signal_event_->setPosition (scene_to_signal_factor * signal_event_->getPosition());
    signal_event_->setDuration (scene_to_signal_factor * signal_event_->getDuration());
}

//-----------------------------------------------------------------------------
NewEventUndoCommand::~NewEventUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void NewEventUndoCommand::undo ()
{
    signal_browser_model_.removeEvent(signal_event_->getId());
}

//-----------------------------------------------------------------------------
void NewEventUndoCommand::redo ()
{
    EventGraphicsItem* event_item
        = signal_browser_model_.addEvent(signal_event_);
    //signal_browser_model_.unsetSelectedEventItem();
}
}
