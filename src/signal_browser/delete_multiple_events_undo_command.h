#ifndef DELETE_MULTIPLE_EVENTS_UNDO_COMMAND_H
#define DELETE_MULTIPLE_EVENTS_UNDO_COMMAND_H

#include "../base/signal_event.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

class EventManager;

//-----------------------------------------------------------------------------
/// DeleteMultipleEventsUndoCommand
///
/// command for removing multiple events at once
class DeleteMultipleEventsUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    DeleteMultipleEventsUndoCommand (EventManager& event_manager,
                                     QList<EventID> const& event_ids);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~DeleteMultipleEventsUndoCommand ();

    //-------------------------------------------------------------------------
    /// recreates the deleted events
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the events
    virtual void redo ();


private:
    EventManager& event_manager_;
    QList<QSharedPointer<SignalEvent const> > deleted_events_;


    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    DeleteMultipleEventsUndoCommand (DeleteMultipleEventsUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    DeleteMultipleEventsUndoCommand& operator= (DeleteMultipleEventsUndoCommand const &);

};

}


#endif // DELETE_MULTIPLE_EVENTS_UNDO_COMMAND_H
