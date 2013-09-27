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


#include "resize_event_undo_command.h"
#include "file_handling/event_manager.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
ResizeEventUndoCommand::ResizeEventUndoCommand (QSharedPointer<EventManager> event_manager,
                                                EventID id,
                                                uint32 new_start_position,
                                                uint32 new_duration)
: event_manager_ (event_manager),
  event_id_ (id),
  new_start_position_ (new_start_position),
  new_duration_ (new_duration)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ResizeEventUndoCommand::undo ()
{
    QSharedPointer<SignalEvent> event = event_manager_->getAndLockEventForEditing (event_id_);
    event->setDuration (old_duration_);
    event->setPosition (old_start_position_);
    event_manager_->updateAndUnlockEvent (event->getId());
}

//-----------------------------------------------------------------------------
void ResizeEventUndoCommand::redo ()
{
    QSharedPointer<SignalEvent> event = event_manager_->getAndLockEventForEditing (event_id_);
    old_duration_ = event->getDuration();
    old_start_position_ = event->getPosition();
    event->setDuration (new_duration_);
    event->setPosition (new_start_position_);
    event_manager_->updateAndUnlockEvent (event->getId());
}



}
