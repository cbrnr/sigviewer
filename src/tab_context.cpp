// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#include "tab_context.h"

#include <QDebug>

namespace SigViewer_
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
    emit selectionStateChanged (NO_TAB_SELECTION_STATE);
    emit editStateChanged (NO_TAB_EDIT_STATE);
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


} // namespace SigViewer_
