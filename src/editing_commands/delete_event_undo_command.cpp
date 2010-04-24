#include "delete_event_undo_command.h"
#include "../file_handling/event_manager.h"

namespace BioSig_
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
