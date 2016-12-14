#ifndef NEW_EVENT_UNDO_COMMAND_H
#define NEW_EVENT_UNDO_COMMAND_H

#include "base/signal_event.h"
#include "file_handling/event_manager.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace SigViewer_
{

class NewEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    NewEventUndoCommand (QSharedPointer<EventManager> event_manager,
                         QSharedPointer<SignalEvent const> signal_event,
                         float scene_to_signal_factor = 1);

    //-------------------------------------------------------------------------
    virtual ~NewEventUndoCommand ();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();

    //-------------------------------------------------------------------------
    EventID getIDOfNewlyCreatedEvent () const {return created_signal_event_->getId();}

private:
    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<SignalEvent> raw_signal_event_;
    QSharedPointer<SignalEvent const> created_signal_event_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    NewEventUndoCommand (NewEventUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    NewEventUndoCommand& operator= (NewEventUndoCommand const &);

};

}

#endif // NEW_EVENT_UNDO_COMMAND_H
