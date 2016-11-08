#ifndef RESIZE_EVENT_UNDO_COMMAND_H
#define RESIZE_EVENT_UNDO_COMMAND_H

#include "base/signal_event.h"
#include "file_handling/event_manager.h"

#include <QSharedPointer>
#include <QUndoCommand>

namespace SigViewer_
{

class ResizeEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    ResizeEventUndoCommand (QSharedPointer<EventManager> event_manager,
                            EventID id,
                            uint32 new_start_position, uint32 new_duration);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~ResizeEventUndoCommand () {}

    //-------------------------------------------------------------------------
    /// undos the resizing
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// resizes the event
    virtual void redo ();

private:
    QSharedPointer<EventManager> event_manager_;

    EventID event_id_;
    uint32 new_start_position_;
    uint32 new_duration_;
    uint32 old_start_position_;
    uint32 old_duration_;

};

}

#endif // RESIZE_EVENT_UNDO_COMMAND_H
