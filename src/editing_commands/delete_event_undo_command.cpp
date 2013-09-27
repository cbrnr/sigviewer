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


#include "delete_event_undo_command.h"
#include "file_handling/event_manager.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
DeleteEventUndoCommand::DeleteEventUndoCommand (QSharedPointer<EventManager> event_manager,
                                                EventID event_id)
    : event_manager_ (event_manager),
      deleted_event_ (event_manager->getEvent (event_id))
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
DeleteEventUndoCommand::~DeleteEventUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void DeleteEventUndoCommand::undo ()
{
    event_manager_->createEvent (
            deleted_event_->getChannel (),
            deleted_event_->getPosition (),
            deleted_event_->getDuration (),
            deleted_event_->getType (),
            deleted_event_->getId ());
}

//-----------------------------------------------------------------------------
void DeleteEventUndoCommand::redo ()
{
    event_manager_->removeEvent (deleted_event_->getId ());
}

}
