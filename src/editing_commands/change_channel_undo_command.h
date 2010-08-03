#ifndef CHANGE_CHANNEL_UNDO_COMMAND_H
#define CHANGE_CHANNEL_UNDO_COMMAND_H

#include "base/signal_event.h"
#include "file_handling/event_manager.h"

#include <QUndoCommand>
#include <QSharedPointer>

namespace SigViewer_
{

class ChangeChannelUndoCommand : public QUndoCommand
{
public:
    //-------------------------------------------------------------------------
    ChangeChannelUndoCommand (QSharedPointer<EventManager> event_manager,
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
    /// copy-constructor disabled
    ChangeChannelUndoCommand (ChangeChannelUndoCommand const &);

    //-------------------------------------------------------------------------
    /// assignment-operator disabled
    ChangeChannelUndoCommand& operator= (ChangeChannelUndoCommand const &);

    QSharedPointer<EventManager> event_manager_;
    EventID event_id_;
    ChannelID new_channel_;
    ChannelID old_channel_;

};

}

#endif // CHANGE_CHANNEL_UNDO_COMMAND_H
