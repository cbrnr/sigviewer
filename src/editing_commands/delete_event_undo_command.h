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


#ifndef DELETE_EVENT_UNDO_COMMAND_H
#define DELETE_EVENT_UNDO_COMMAND_H

#include "base/signal_event.h"
#include "file_handling/event_manager.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace sigviewer
{

class DeleteEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    DeleteEventUndoCommand (QSharedPointer<EventManager> event_manager,
                            EventID event_id);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~DeleteEventUndoCommand ();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();


private:
    QSharedPointer<EventManager> event_manager_;
    EventID event_id_;
    QSharedPointer<SignalEvent const> deleted_event_;


    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    DeleteEventUndoCommand (DeleteEventUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    DeleteEventUndoCommand& operator= (DeleteEventUndoCommand const &);

};

}

#endif // DELETE_EVENT_UNDO_COMMAND_H
