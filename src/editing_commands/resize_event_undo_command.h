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


#ifndef RESIZE_EVENT_UNDO_COMMAND_H
#define RESIZE_EVENT_UNDO_COMMAND_H

#include "base/signal_event.h"
#include "file_handling/event_manager.h"

#include <QSharedPointer>
#include <QUndoCommand>

namespace sigviewer
{

class ResizeEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    ResizeEventUndoCommand (QSharedPointer<EventManager> event_manager,
                            EventID id,
                            uint32 new_start_position, uint32 new_duration);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~ResizeEventUndoCommand () {}

    //-------------------------------------------------------------------------
    /// undos the resizing
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// resizes the event
    virtual void redo ();

private:
    QSharedPointer<EventManager> event_manager_;

    EventID event_id_;
    uint32 new_start_position_;
    uint32 new_duration_;
    uint32 old_start_position_;
    uint32 old_duration_;

};

}

#endif // RESIZE_EVENT_UNDO_COMMAND_H
