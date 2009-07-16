#include "delete_event_undo_command.h"

namespace BioSig_
{

namespace PortingToQT4_
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
    event_item_->show();
}

//-----------------------------------------------------------------------------
void DeleteEventUndoCommand::redo ()
{
    event_item_->hide();
}

}

}
