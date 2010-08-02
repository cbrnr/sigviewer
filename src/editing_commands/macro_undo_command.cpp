#include "macro_undo_command.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
MacroUndoCommand::MacroUndoCommand (QList<QSharedPointer<QUndoCommand> > const& commands)
    : commands_ (commands)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void MacroUndoCommand::undo ()
{
    for (int index = commands_.length () - 1; index > -1; index--)
        commands_[index]->undo ();
}

//-----------------------------------------------------------------------------
void MacroUndoCommand::redo ()
{
    for (int index = 0; index < commands_.length (); index++)
        commands_[index]->redo ();
}

} // namespace SigViewer_
