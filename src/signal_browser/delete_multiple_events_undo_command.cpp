#include "delete_multiple_events_undo_command.h"
#include "event_manager_interface.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
DeleteMultipleEventsUndoCommand::DeleteMultipleEventsUndoCommand (EventManagerInterface& event_manager,
                                               QList<EventID> const& event_ids)
    : event_manager_ (event_manager)
{
    for (QList<EventID>::ConstIterator id_iter = event_ids.begin();
         id_iter != event_ids.end ();
         ++id_iter)
        deleted_events_.append (event_manager_.getEvent (*id_iter));
}

//-----------------------------------------------------------------------------
DeleteMultipleEventsUndoCommand::~DeleteMultipleEventsUndoCommand ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void DeleteMultipleEventsUndoCommand::undo ()
{
    for (QList<QSharedPointer<SignalEvent const> >::Iterator event_iter = deleted_events_.begin ();
         event_iter != deleted_events_.end ();
         ++event_iter)
    {
        event_manager_.createEvent (
            (*event_iter)->getChannel (),
            (*event_iter)->getPosition (),
            (*event_iter)->getDuration (),
            (*event_iter)->getType (),
            (*event_iter)->getId ());
    }
}

//-----------------------------------------------------------------------------
void DeleteMultipleEventsUndoCommand::redo ()
{
    for (QList<QSharedPointer<SignalEvent const> >::Iterator event_iter = deleted_events_.begin ();
         event_iter != deleted_events_.end ();
         ++event_iter)
        event_manager_.removeEvent ((*event_iter)->getId ());
}

}
