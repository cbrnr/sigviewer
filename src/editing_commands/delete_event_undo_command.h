#ifndef DELETE_EVENT_UNDO_COMMAND_H
#define DELETE_EVENT_UNDO_COMMAND_H

#include "../base/signal_event.h"
#include "../file_handling/event_manager.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

class DeleteEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    DeleteEventUndoCommand (QSharedPointer<EventManager> event_manager,
                            EventID event_id);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~DeleteEventUndoCommand ();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();


private:
    QSharedPointer<EventManager> event_manager_;
    EventID event_id_;
    QSharedPointer<SignalEvent const> deleted_event_;


    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    DeleteEventUndoCommand (DeleteEventUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    DeleteEventUndoCommand& operator= (DeleteEventUndoCommand const &);

};

}

#endif // DELETE_EVENT_UNDO_COMMAND_H
