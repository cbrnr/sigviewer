#ifndef RESIZE_EVENT_UNDO_COMMAND_H
#define RESIZE_EVENT_UNDO_COMMAND_H

#include "../base/signal_event.h"

#include <QSharedPointer>
#include <QUndoCommand>

namespace BioSig_
{

class EventManager;

class ResizeEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    ResizeEventUndoCommand (EventManager& event_manager,
                            EventID id,
                            uint32 new_start_position, uint32 new_duration);

    //-------------------------------------------------------------------------
    /// destructor
    virtual ~ResizeEventUndoCommand ();

    //-------------------------------------------------------------------------
    /// undos the resizing
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// resizes the event
    virtual void redo ();

private:
    EventManager& event_manager_;
    QSharedPointer<SignalEvent> event_;

    uint32 new_start_position_;
    uint32 new_duration_;
    uint32 old_start_position_;
    uint32 old_duration_;

};

}

#endif // RESIZE_EVENT_UNDO_COMMAND_H
