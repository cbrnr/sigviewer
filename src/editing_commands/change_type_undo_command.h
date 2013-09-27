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


#ifndef CHANGE_TYPE_UNDO_COMMAND_H
#define CHANGE_TYPE_UNDO_COMMAND_H

#include "base/signal_event.h"
#include "file_handling/event_manager.h"

#include <QUndoCommand>
#include <QSharedPointer>
#include <QObject>

namespace SigViewer_
{

class ChangeTypeUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    ChangeTypeUndoCommand (QSharedPointer<EventManager> event_manager,
                           EventID event_id,
                           EventType new_type);

    //-------------------------------------------------------------------------
    virtual ~ChangeTypeUndoCommand();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();

private:
    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    ChangeTypeUndoCommand (ChangeTypeUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    ChangeTypeUndoCommand& operator= (ChangeTypeUndoCommand const &);

    EventID event_id_;
    QSharedPointer<EventManager> event_manager_;
    EventType new_type_;
    EventType old_type_;
    QSharedPointer<SignalEvent> signal_event_;
};

}

#endif // CHANGE_TYPE_UNDO_COMMAND_H
