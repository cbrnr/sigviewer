#ifndef NEW_EVENT_UNDO_COMMAND_H
#define NEW_EVENT_UNDO_COMMAND_H

#include "../base/signal_event.h"
#include "signal_browser_model_4.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

class NewEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    NewEventUndoCommand (SignalBrowserModel& signal_browser_model,
                         QSharedPointer<SignalEvent> signal_event,
                         float scene_to_signal_factor = 1);

    //-------------------------------------------------------------------------
    virtual ~NewEventUndoCommand ();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();


private:
    SignalBrowserModel& signal_browser_model_;
    //QSharedPointer<EventGraphicsItem> event_item_;
    QSharedPointer<SignalEvent> signal_event_;

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    NewEventUndoCommand (NewEventUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    NewEventUndoCommand& operator= (NewEventUndoCommand const &);

};

}

#endif // NEW_EVENT_UNDO_COMMAND_H
