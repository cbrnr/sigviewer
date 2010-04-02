#include "change_type_undo_command.h"
#include "event_manager_interface.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
ChangeTypeUndoCommand::ChangeTypeUndoCommand (EventManagerInterface& event_manager,
                                              EventID event_id,
                                              EventType new_type)
 : event_manager_ (event_manager),
   new_type_ (new_type),
   signal_event_ (event_manager.getEventForEditing (event_id))
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
    event_manager_.updateEvent (signal_event_->getId());
}

//-----------------------------------------------------------------------------
void ChangeTypeUndoCommand::redo ()
{
    old_type_ = signal_event_->getType();
    signal_event_->setType(new_type_);
    event_manager_.updateEvent (signal_event_->getId());
}

}
