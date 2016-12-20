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


#ifndef NEW_EVENT_UNDO_COMMAND_H
#define NEW_EVENT_UNDO_COMMAND_H

#include "base/signal_event.h"
#include "file_handling/event_manager.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace sigviewer
{

class NewEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    NewEventUndoCommand (QSharedPointer<EventManager> event_manager,
                         QSharedPointer<SignalEvent const> signal_event,
                         float scene_to_signal_factor = 1);

    //-------------------------------------------------------------------------
    virtual ~NewEventUndoCommand ();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();

    //-------------------------------------------------------------------------
    EventID getIDOfNewlyCreatedEvent () const {return created_signal_event_->getId();}

private:
    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<SignalEvent> raw_signal_event_;
    QSharedPointer<SignalEvent const> created_signal_event_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    NewEventUndoCommand (NewEventUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    NewEventUndoCommand& operator= (NewEventUndoCommand const &);

};

}

#endif // NEW_EVENT_UNDO_COMMAND_H
