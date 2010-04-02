#include "change_channel_undo_command.h"
#include "../file_handling/event_manager.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ChangeChannelUndoCommand::ChangeChannelUndoCommand (EventManager& event_manager,
                                                    EventID event_id,
                                                    ChannelID new_channel)
 : event_manager_ (event_manager),
   signal_event_ (event_manager.getEventForEditing (event_id)),
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
    signal_event_->setChannel(old_channel_);
    event_manager_.updateEvent (signal_event_->getId());
}

//-----------------------------------------------------------------------------
void ChangeChannelUndoCommand::redo ()
{
    old_channel_ = signal_event_->getChannel();
    signal_event_->setChannel (new_channel_);
    event_manager_.updateEvent (signal_event_->getId());
}

}
