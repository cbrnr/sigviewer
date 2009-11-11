#include "next_event_view_undo_command.h"

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
    /// TODO: implement
}

//-----------------------------------------------------------------------------
void NextEventViewUndoCommand::redo ()
{
    signal_browser_model_.goToAndSelectNextEvent();
}


}
