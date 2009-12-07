#include "command_stack.h"

#include <iostream>

namespace BioSig_
{

//-----------------------------------------------------------------------------
CommandStack& CommandStack::instance ()
{
    static CommandStack command_stack;
    return command_stack;
}

//-------------------------------------------------------------------------
void CommandStack::clearStacks ()
{
    edit_command_undo_stack_.clear();
    view_command_undo_stack_.clear();
}

//-----------------------------------------------------------------------------
void CommandStack::executeEditCommand (QUndoCommand* command)
{
    edit_command_undo_stack_.push (command);
}

//-----------------------------------------------------------------------------
void CommandStack::undoLastEditCommand ()
{
    edit_command_undo_stack_.undo ();
}

//-----------------------------------------------------------------------------
void CommandStack::redoLastUndoneEditCommand ()
{
    edit_command_undo_stack_.redo ();
}

//-----------------------------------------------------------------------------
bool CommandStack::emptyEditCommandStack ()
{
    return !(edit_command_undo_stack_.canUndo());
}

//-----------------------------------------------------------------------------
void CommandStack::executeViewCommand (QUndoCommand* command)
{
    view_command_undo_stack_.push (command);
}

//-----------------------------------------------------------------------------
void CommandStack::undoLastViewCommand ()
{
    view_command_undo_stack_.undo ();
}

//-----------------------------------------------------------------------------
void CommandStack::redoLastUndoneViewCommand ()
{
    view_command_undo_stack_.redo ();
}


//-----------------------------------------------------------------------------
CommandStack::CommandStack ()
{
    // nothing to do here
}

}
