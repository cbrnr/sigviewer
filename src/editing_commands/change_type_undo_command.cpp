#include "change_type_undo_command.h"
#include "file_handling/event_manager.h"

namespace SigViewer_
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
