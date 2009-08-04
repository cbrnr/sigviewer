#include "command_stack.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
CommandStack& CommandStack::instance ()
{
    static CommandStack command_stack;
    return command_stack;
}

//-----------------------------------------------------------------------------
void CommandStack::executeCommand (QUndoCommand* command)
{
    undo_stack_.push (command);
}

//-----------------------------------------------------------------------------
void CommandStack::undoLastCommand ()
{
    undo_stack_.undo ();
}

//-----------------------------------------------------------------------------
void CommandStack::redoLastUndoneCommand ()
{
    undo_stack_.redo ();
}



//-----------------------------------------------------------------------------
CommandStack::CommandStack ()
{
    // nothing to do here
}

}
