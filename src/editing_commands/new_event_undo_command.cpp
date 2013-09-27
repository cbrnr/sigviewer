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


#include "new_event_undo_command.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
NewEventUndoCommand::NewEventUndoCommand (QSharedPointer<EventManager> event_manager,
                                          QSharedPointer<SignalEvent const> signal_event,
                                          float scene_to_signal_factor)
 : event_manager_ (event_manager),
   raw_signal_event_ (new SignalEvent (*signal_event))
{
    raw_signal_event_->setPosition (scene_to_signal_factor * raw_signal_event_->getPosition());
    raw_signal_event_->setDuration (scene_to_signal_factor * raw_signal_event_->getDuration());
}

//-----------------------------------------------------------------------------
NewEventUndoCommand::~NewEventUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void NewEventUndoCommand::undo ()
{
    event_manager_->removeEvent (created_signal_event_->getId ());
}

//-----------------------------------------------------------------------------
void NewEventUndoCommand::redo ()
{
    EventID id = UNDEFINED_EVENT_ID;
    if (!created_signal_event_.isNull())
        id = created_signal_event_->getId ();

    created_signal_event_ = event_manager_->createEvent (
            raw_signal_event_->getChannel(),
            raw_signal_event_->getPosition(),
            raw_signal_event_->getDuration(),
            raw_signal_event_->getType(),
            id);
}

}
