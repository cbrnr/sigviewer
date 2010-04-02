#ifndef CHANGE_CHANNEL_UNDO_COMMAND_H
#define CHANGE_CHANNEL_UNDO_COMMAND_H

#include "../base/signal_event.h"
#include "channel_manager_interface.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

class EventManager;

class ChangeChannelUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    ChangeChannelUndoCommand (EventManager& event_manager,
                              EventID event_id,
                              ChannelID new_channel);

    //-------------------------------------------------------------------------
    virtual ~ChangeChannelUndoCommand();

    //-------------------------------------------------------------------------
    /// recreates the deleted event
    virtual void undo ();

    //-------------------------------------------------------------------------
    /// deletes the event
    virtual void redo ();

private:
    //-------------------------------------------------------------------------
    void checkAndSetSelectChannelState ();

    //-------------------------------------------------------------------------
    /// copy-constructor disabled
    ChangeChannelUndoCommand (ChangeChannelUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    ChangeChannelUndoCommand& operator= (ChangeChannelUndoCommand const &);

    EventManager& event_manager_;
    QSharedPointer<SignalEvent> signal_event_;
    ChannelID new_channel_;
    ChannelID old_channel_;

};

}

#endif // CHANGE_CHANNEL_UNDO_COMMAND_H
