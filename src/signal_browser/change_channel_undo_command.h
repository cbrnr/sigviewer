#ifndef CHANGE_CHANNEL_UNDO_COMMAND_H
#define CHANGE_CHANNEL_UNDO_COMMAND_H

#include "../base/signal_event.h"
#include "signal_browser_model_4.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace BioSig_
{

class ChangeChannelUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    ChangeChannelUndoCommand(SignalBrowserModel& signal_browser_model,
                                          QSharedPointer<SignalEvent> signal_event,
                                          int32 new_channel);

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

    SignalBrowserModel& signal_browser_model_;
    QSharedPointer<SignalEvent> signal_event_;
    int32 new_channel_;
    int32 old_channel_;

};

}

#endif // CHANGE_CHANNEL_UNDO_COMMAND_H
