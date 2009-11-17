#include "set_shown_event_types_view_undo_command.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
SetShownEventTypesViewUndoCommand::SetShownEventTypesViewUndoCommand (SignalBrowserModel& signal_browser_model, SignalBrowserModel::IntList const& event_types_to_show)
: signal_browser_model_ (signal_browser_model),
  new_event_types_to_show_ (event_types_to_show)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
SetShownEventTypesViewUndoCommand::~SetShownEventTypesViewUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void SetShownEventTypesViewUndoCommand::undo ()
{
    signal_browser_model_.setShownEventTypes (previously_shown_event_types_);
    signal_browser_model_.updateEventItems ();
}

//-----------------------------------------------------------------------------
void SetShownEventTypesViewUndoCommand::redo ()
{
    signal_browser_model_.getShownEventTypes (previously_shown_event_types_);
    signal_browser_model_.setShownEventTypes (new_event_types_to_show_);
    signal_browser_model_.updateEventItems ();
}


}
