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


#ifndef TAB_CONTEXT_H
#define TAB_CONTEXT_H

#include "base/tab_states.h"
#include "command_executer.h"

#include <QObject>
#include <QUndoStack>
#include <QUndoCommand>

namespace sigviewer
{

class TabContext : public QObject, public CommandExecuter
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    TabContext ();

    //-------------------------------------------------------------------------
    ~TabContext ();

    //-------------------------------------------------------------------------
    void gotActive ();

    //-------------------------------------------------------------------------
    virtual void executeCommand (QUndoCommand* command);

    //-------------------------------------------------------------------------
    virtual void undo ();

    //-------------------------------------------------------------------------
    virtual void redo ();

signals:
    //-------------------------------------------------------------------------
    void selectionStateChanged (TabSelectionState state);

    //-------------------------------------------------------------------------
    void editStateChanged (TabEditState state);

public slots:
    //-------------------------------------------------------------------------
    void setSelectionState (TabSelectionState state);

    //-------------------------------------------------------------------------
    void setEditState (TabEditState state);

private:
    //-------------------------------------------------------------------------
    // disabled
    TabContext (TabContext const&);
    TabContext& operator= (TabContext const&);

    //-------------------------------------------------------------------------
    void updateUndoRedoEditState ();


    TabSelectionState selection_state_;
    TabEditState edit_state_;

    QUndoStack edit_undo_stack_;
};

}

#endif // TAB_CONTEXT_H
