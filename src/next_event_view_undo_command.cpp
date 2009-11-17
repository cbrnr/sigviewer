#include "next_event_view_undo_command.h"
#include "signal_browser/event_graphics_item.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
NextEventViewUndoCommand::NextEventViewUndoCommand(SignalBrowserModel& signal_browser_model)
    : signal_browser_model_ (signal_browser_model)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
NextEventViewUndoCommand::~NextEventViewUndoCommand ()
{

}

//-----------------------------------------------------------------------------
void NextEventViewUndoCommand::undo ()
{
    signal_browser_model_.setViewingPosition(previous_view_position_);
    previously_selected_event_->setSelected(true);
}

//-----------------------------------------------------------------------------
void NextEventViewUndoCommand::redo ()
{
    previously_selected_event_ = signal_browser_model_.getSelectedEventItem();
    previous_view_position_ = signal_browser_model_.getViewingPosition();

    signal_browser_model_.goToAndSelectNextEvent();
}


}
