#ifndef CHANGE_TYPE_UNDO_COMMAND_H
#define CHANGE_TYPE_UNDO_COMMAND_H

#include "../base/signal_event.h"

#include <QUndoCommand>
#include <QSharedPointer>
#include <QObject>

namespace BioSig_
{

class ChangeTypeUndoCommand : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    ChangeTypeUndoCommand (QSharedPointer<SignalEvent> signal_event,
                           uint16 new_type);

    //-------------------------------------------------------------------------
    virtual ~ChangeTypeUndoCommand();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();

signals:
    void eventChanged (int32 event_id);

private:
    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    ChangeTypeUndoCommand (ChangeTypeUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    ChangeTypeUndoCommand& operator= (ChangeTypeUndoCommand const &);

    QSharedPointer<SignalEvent> signal_event_;
    uint16 new_type_;
    uint16 old_type_;
};

}

#endif // CHANGE_TYPE_UNDO_COMMAND_H
