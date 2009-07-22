#include "delete_event_undo_command.h"

#include <iostream>

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
    std::cout << "DeleteEventUndoCommand::undo not implemented yet!!!" << std::endl;
    //signal_browser_model_.addEvent(event_item_);
    event_item_->show();
}

//-----------------------------------------------------------------------------
void DeleteEventUndoCommand::redo ()
{
    std::cout << "DeleteEventUndoCommand::redo not implemented yet!!!" << std::endl;
    signal_browser_model_.removeEvent(event_item_->getId());
    // event_item_->hide();
}

}

}
