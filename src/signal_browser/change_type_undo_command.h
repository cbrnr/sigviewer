#ifndef CHANGE_TYPE_UNDO_COMMAND_H
#define CHANGE_TYPE_UNDO_COMMAND_H

#include "../base/signal_event.h"

#include <QUndoCommand>
#include <QSharedPointer>
#include <QObject>

namespace BioSig_
{

class EventManagerInterface;

class ChangeTypeUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    ChangeTypeUndoCommand (EventManagerInterface& event_manager,
                           EventID event_id,
                           EventType new_type);

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

    EventManagerInterface& event_manager_;
    EventType new_type_;
    EventType old_type_;
    QSharedPointer<SignalEvent> signal_event_;
};

}

#endif // CHANGE_TYPE_UNDO_COMMAND_H
