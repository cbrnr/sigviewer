#include "tab_context.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
TabContext::TabContext ()
    : selection_state_ (TAB_STATE_NO_EVENT_SELECTED),
      edit_state_ (TAB_STATE_NO_REDO_NO_UNDO)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
TabContext::~TabContext ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void TabContext::gotActive ()
{
    emit selectionStateChanged (selection_state_);
    emit editStateChanged (edit_state_);
}

//-----------------------------------------------------------------------------
void TabContext::executeCommand (QUndoCommand* command)
{
    edit_undo_stack_.push (command);
    updateUndoRedoEditState ();
}

//-------------------------------------------------------------------------
void TabContext::undo ()
{
    edit_undo_stack_.undo ();
    updateUndoRedoEditState ();
}

//-------------------------------------------------------------------------
void TabContext::redo ()
{
    edit_undo_stack_.redo ();
    updateUndoRedoEditState ();
}

//-----------------------------------------------------------------------------
void TabContext::updateUndoRedoEditState ()
{
    if (!edit_undo_stack_.canUndo () && !edit_undo_stack_.canRedo())
        setEditState (TAB_STATE_NO_REDO_NO_UNDO);
    else if (!edit_undo_stack_.canUndo ())
        setEditState (TAB_STATE_NO_UNDO);
    else if (!edit_undo_stack_.canRedo())
        setEditState (TAB_STATE_NO_REDO);
    else
        setEditState (TAB_STATE_CAN_REDO_UNDO);
}



//-----------------------------------------------------------------------------
void TabContext::setSelectionState (TabSelectionState state)
{
    selection_state_ = state;
    emit selectionStateChanged (selection_state_);
}

//-----------------------------------------------------------------------------
void TabContext::setEditState (TabEditState state)
{
    edit_state_ = state;
    emit editStateChanged (edit_state_);
}


} // namespace BioSig_
