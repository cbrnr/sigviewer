#ifndef RESIZE_EVENT_UNDO_COMMAND_H
#define RESIZE_EVENT_UNDO_COMMAND_H

#include "../base/signal_event.h"
#include "signal_browser_model_4.h"

#include <QSharedPointer>
#include <QUndoCommand>

namespace BioSig_
{

namespace PortingToQT4_
{

class ResizeEventUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    /// constructor
    ResizeEventUndoCommand (SignalBrowserModel& signal_browser_model, QSharedPointer<SignalEvent> event, uint32 new_start_position, uint32 new_duration);

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
    SignalBrowserModel& signal_browser_model_;
    QSharedPointer<SignalEvent> event_;

    uint32 new_start_position_;
    uint32 new_duration_;
    uint32 old_start_position_;
    uint32 old_duration_;

};

}

}

#endif // RESIZE_EVENT_UNDO_COMMAND_H
