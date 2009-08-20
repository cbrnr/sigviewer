#ifndef CHANGE_TYPE_UNDO_COMMAND_H
#define CHANGE_TYPE_UNDO_COMMAND_H

#include "../base/signal_event.h"
#include "signal_browser_model_4.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

namespace PortingToQT4_
{

class ChangeTypeUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    ChangeTypeUndoCommand (SignalBrowserModel& signal_browser_model,
                           QSharedPointer<SignalEvent> signal_event,
                           uint16 new_type);

    //-------------------------------------------------------------------------
    virtual ~ChangeTypeUndoCommand();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();

private:
    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    ChangeTypeUndoCommand (ChangeTypeUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    ChangeTypeUndoCommand& operator= (ChangeTypeUndoCommand const &);

    SignalBrowserModel& signal_browser_model_;
    QSharedPointer<SignalEvent> signal_event_;
    uint16 new_type_;
    uint16 old_type_;
};

}

}


#endif // CHANGE_TYPE_UNDO_COMMAND_H
