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


#include "change_type_undo_command.h"
#include "file_handling/event_manager.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
ChangeTypeUndoCommand::ChangeTypeUndoCommand (QSharedPointer<EventManager> event_manager,
                                              EventID event_id,
                                              EventType new_type)
 : event_id_ (event_id),
   event_manager_ (event_manager),
   new_type_ (new_type)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
ChangeTypeUndoCommand::~ChangeTypeUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ChangeTypeUndoCommand::undo ()
{
    signal_event_->setType(old_type_);
    event_manager_->updateAndUnlockEvent (signal_event_->getId());
}

//-----------------------------------------------------------------------------
void ChangeTypeUndoCommand::redo ()
{
    signal_event_ = event_manager_->getAndLockEventForEditing (event_id_);
    old_type_ = signal_event_->getType();
    signal_event_->setType(new_type_);
    event_manager_->updateAndUnlockEvent (signal_event_->getId());
}

}
