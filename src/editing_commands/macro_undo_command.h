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


#ifndef MACRO_UNDO_COMMAND_H
#define MACRO_UNDO_COMMAND_H

#include <QUndoCommand>
#include <QSharedPointer>

namespace SigViewer_
{

class MacroUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    MacroUndoCommand (QList<QSharedPointer<QUndoCommand> > const& commands);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~MacroUndoCommand () {}

    //-------------------------------------------------------------------------
    /// undos the all commands
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// executes the macro
    virtual void redo ();


private:
    QList<QSharedPointer<QUndoCommand> > commands_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    MacroUndoCommand (MacroUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    MacroUndoCommand& operator= (MacroUndoCommand const &);

};

}

#endif // MACRO_UNDO_COMMAND_H
