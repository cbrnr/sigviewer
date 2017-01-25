// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "new_event_undo_command.h"
#include "file_handling_impl/xdf_reader.h"

namespace sigviewer
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

    //If XDF file, also pop back the event added to the XDFdata object earlier
    if (event_manager_->getFileType().startsWith("XDF", Qt::CaseInsensitive))
    {
        XDFdata.userCreatedEvents.pop_back();
        if (XDFdata.userCreatedEvents.empty())
        {
            XDFdata.streams.pop_back();
            XDFdata.userAddedStream = 0;
        }
    }
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
