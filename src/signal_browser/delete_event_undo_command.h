#ifndef DELETE_EVENT_UNDO_COMMAND_H
#define DELETE_EVENT_UNDO_COMMAND_H

#include "signal_browser_model_4.h"
#include "event_graphics_item.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

class DeleteEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    ///
    /// @param
    DeleteEventUndoCommand (SignalBrowserModel& signal_browser_model, QSharedPointer<EventGraphicsItem> event_item);

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
    SignalBrowserModel& signal_browser_model_;
    QSharedPointer<EventGraphicsItem> event_item_;


    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    DeleteEventUndoCommand (DeleteEventUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    DeleteEventUndoCommand& operator= (DeleteEventUndoCommand const &);

};

}

#endif // DELETE_EVENT_UNDO_COMMAND_H
