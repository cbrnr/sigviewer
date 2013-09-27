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
