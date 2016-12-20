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


#include "change_channel_undo_command.h"
#include "file_handling/event_manager.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
ChangeChannelUndoCommand::ChangeChannelUndoCommand (QSharedPointer<EventManager> event_manager,
                                                    EventID event_id,
                                                    ChannelID new_channel)
 : event_manager_ (event_manager),
   event_id_ (event_id),
   new_channel_ (new_channel)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
ChangeChannelUndoCommand::~ChangeChannelUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void ChangeChannelUndoCommand::undo ()
{
    QSharedPointer<SignalEvent> signal_event_ = event_manager_->getAndLockEventForEditing (event_id_);
    signal_event_->setChannel (old_channel_);
    event_manager_->updateAndUnlockEvent (signal_event_->getId());
}

//-----------------------------------------------------------------------------
void ChangeChannelUndoCommand::redo ()
{
    QSharedPointer<SignalEvent> signal_event_ = event_manager_->getAndLockEventForEditing (event_id_);
    old_channel_ = signal_event_->getChannel();
    signal_event_->setChannel (new_channel_);
    event_manager_->updateAndUnlockEvent (signal_event_->getId());
}

}
