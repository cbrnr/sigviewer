#include "delete_event_undo_command.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
DeleteEventUndoCommand::DeleteEventUndoCommand(SignalBrowserModel& signal_browser_model, QSharedPointer<EventGraphicsItem> event_item)
    : signal_browser_model_ (signal_browser_model),
      event_item_ (event_item)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
DeleteEventUndoCommand::~DeleteEventUndoCommand ()
{

}

//-----------------------------------------------------------------------------
void DeleteEventUndoCommand::undo ()
{
    signal_browser_model_.addEvent(event_item_);
}

//-----------------------------------------------------------------------------
void DeleteEventUndoCommand::redo ()
{
    signal_browser_model_.removeEvent(event_item_->getId());
}

}
