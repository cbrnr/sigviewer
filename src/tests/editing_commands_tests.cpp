#include "editing_commands_tests.h"
#include "editing_commands/change_channel_undo_command.h"
#include "editing_commands/change_type_undo_command.h"
#include "editing_commands/delete_event_undo_command.h"
#include "editing_commands/new_event_undo_command.h"
#include "editing_commands/resize_event_undo_command.h"

#include <QUndoStack>

namespace SigViewer_
{

namespace Tests_
{

QString EditingCommandsTests::run ()
{
    QSharedPointer<EventManager> event_manager = createEventManagerWithDummyData();
    QSharedPointer<SignalEvent const> event = event_manager->getEvent (1);
    EventID id = event->getId();

    QUndoStack stack;

    unsigned const initial_number_of_events = event_manager->getNumberOfEvents();
    ChannelID const old_channel = event->getChannel();
    EventType const old_type = event->getType();
    uint32 const old_position = event->getPosition();
    uint32 const old_duration = event->getDuration();
    ChannelID const new_channel = 1;
    EventType const new_type = 4;
    uint32 const new_position = 0;
    uint32 const new_duration = 250;

    QUndoCommand* change_channel = new ChangeChannelUndoCommand  (event_manager, id, new_channel);
    QUndoCommand* change_type = new ChangeTypeUndoCommand (event_manager, id, new_type);
    QUndoCommand* delete_event = new DeleteEventUndoCommand (event_manager, id);
    QUndoCommand* new_event = new NewEventUndoCommand (event_manager, QSharedPointer<SignalEvent>(new SignalEvent (10, 10, 100, 1, 100)), 1);
    QUndoCommand* resize_event = new ResizeEventUndoCommand (event_manager, id, new_position, new_duration);

    stack.push (change_channel);
    VERIFY (event_manager->getEvent(id)->getChannel() == new_channel, "change channel");
    stack.undo();
    VERIFY (event_manager->getEvent(id)->getChannel() == old_channel, "change channel undo");

    stack.push (change_type);
    VERIFY (event_manager->getEvent(id)->getType() == new_type, "change type");
    stack.undo();
    VERIFY (event_manager->getEvent(id)->getType() == old_type, "change type undo");

    stack.push (delete_event);
    VERIFY (event_manager->getEvent(id).isNull(), "delete event");
    VERIFY (event_manager->getNumberOfEvents() == (initial_number_of_events - 1), "delete event 2");
    stack.undo();
    VERIFY (!event_manager->getEvent(id).isNull(), "delete event undo");
    VERIFY (event_manager->getNumberOfEvents() == initial_number_of_events, "delete event undo 2");

    stack.push (new_event);
    VERIFY (event_manager->getNumberOfEvents() == (initial_number_of_events + 1), "new event");
    stack.undo ();
    VERIFY (event_manager->getNumberOfEvents() == initial_number_of_events, "new event undo");

    stack.push (resize_event);
    VERIFY (event_manager->getEvent(id)->getPosition() == new_position, "resize event");
    VERIFY (event_manager->getEvent(id)->getDuration() == new_duration, "resize event 2");
    stack.undo ();
    VERIFY (event_manager->getEvent(id)->getPosition() == old_position, "resize event undo");
    VERIFY (event_manager->getEvent(id)->getDuration() == old_duration, "resize event undo 2");

    return "";
}

}

}
