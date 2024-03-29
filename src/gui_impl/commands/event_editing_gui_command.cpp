// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "event_editing_gui_command.h"
#include "gui_impl/gui_helper_functions.h"
#include "editing_commands/delete_event_undo_command.h"
#include "editing_commands/change_type_undo_command.h"
#include "editing_commands/change_channel_undo_command.h"
#include "editing_commands/new_event_undo_command.h"
#include "editing_commands/macro_undo_command.h"

#include <QDebug>

namespace sigviewer
{

//-----------------------------------------------------------------------------
namespace {

class EventEditingGuiCommandFactory: public GuiActionCommandFactory
{
public:
    QSharedPointer<GuiActionCommand> createCommand() override
    {
        return QSharedPointer<EventEditingGuiCommand> (new EventEditingGuiCommand);
    }
};

} // unnamed namespace

QString const EventEditingGuiCommand::DELETE_()
{
    static QString value = tr("Delete");

    return value;
}

QString const EventEditingGuiCommand::CHANGE_TYPE_()
{
    static QString value = tr("Change Type...");

    return value;
}

QString const EventEditingGuiCommand::CHANGE_CHANNEL_()
{
    static QString value = tr("Change Channel...");

    return value;
}

QString const EventEditingGuiCommand::TO_ALL_CHANNEL_()
{
    static QString value = tr("To all Channels");

    return value;
}

QString const EventEditingGuiCommand::COPY_TO_CHANNELS_()
{
    static QString value = tr("Copy to Channels...");

    return value;
}

QString const EventEditingGuiCommand::INSERT_OVER_()
{
    static QString value = tr("Insert Over");

    return value;
}

QStringList const EventEditingGuiCommand::ACTIONS_()
{
    static QStringList result = {
        EventEditingGuiCommand::DELETE_(),
        EventEditingGuiCommand::CHANGE_TYPE_(),
        EventEditingGuiCommand::CHANGE_CHANNEL_(),
        EventEditingGuiCommand::TO_ALL_CHANNEL_(),
        EventEditingGuiCommand::COPY_TO_CHANNELS_(),
        EventEditingGuiCommand::INSERT_OVER_(),
    };

    return result;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator EventEditingGuiCommand::registrator_ ("Event Editing",
                                                                  QSharedPointer<EventEditingGuiCommandFactory> (new EventEditingGuiCommandFactory));


//-----------------------------------------------------------------------------
EventEditingGuiCommand::EventEditingGuiCommand ()
    : GuiActionCommand (ACTIONS_())
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
EventEditingGuiCommand::~EventEditingGuiCommand ()
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
void EventEditingGuiCommand::init ()
{
    resetActionTriggerSlot (DELETE_(), SLOT (deleteSelectedEvent()));
    resetActionTriggerSlot (CHANGE_TYPE_(), SLOT (changeTypeSelectedEvent()));
    resetActionTriggerSlot (CHANGE_CHANNEL_(), SLOT (changeChannelSelectedEvent()));
    resetActionTriggerSlot (TO_ALL_CHANNEL_(), SLOT (toAllChannelsSelectedEvent()));
    resetActionTriggerSlot (COPY_TO_CHANNELS_(), SLOT (copyToChannelsSelectedEvent()));
    resetActionTriggerSlot (INSERT_OVER_(), SLOT (insertEventOverSelectedEvent()));

    getQAction (DELETE_())->setIcon (QIcon(":/images/ic_delete_forever_black_24dp.png"));
    getQAction (TO_ALL_CHANNEL_())->setIcon (QIcon (":/images/ic_font_download_black_24dp.png"));
    getQAction (COPY_TO_CHANNELS_())->setIcon (QIcon (":/images/ic_content_copy_black_24dp.png"));
    getQAction (INSERT_OVER_())->setIcon (QIcon (":/images/ic_add_circle_outline_black_24dp.png"));
    getQAction (DELETE_())->setShortcut (QKeySequence::Delete);
//    getQAction (INSERT_OVER_())->setShortcut (QKeySequence("Ctrl+I"));
}

//-----------------------------------------------------------------------------
void EventEditingGuiCommand::deleteSelectedEvent ()
{
    QList<QSharedPointer<QUndoCommand> > commands;
    foreach (EventID event, GuiHelper::getSelectedEventIDs (currentEventView()))
    {
        commands.append (QSharedPointer<QUndoCommand> (new DeleteEventUndoCommand (
                currentEventView()->getEventManager(),
                event)));
    }
    executeCommands (commands);
}

//-----------------------------------------------------------------------------
void EventEditingGuiCommand::changeTypeSelectedEvent ()
{
    QList<QSharedPointer<QUndoCommand> > commands;
    foreach (EventID event, GuiHelper::getSelectedEventIDs (currentEventView()))
    {
        QSharedPointer<EventManager> event_manager = currentVisModel()->getEventManager();
        EventType pre_selected_type = event_manager->getEvent (event)->getType ();
        EventType new_type = GuiHelper::selectEventType (pre_selected_type, currentVisModel());

        if (new_type == UNDEFINED_EVENT_TYPE)
            return;

        commands.append (QSharedPointer<QUndoCommand> (new ChangeTypeUndoCommand (
                event_manager,
                event,
                new_type)));
    }

    executeCommands (commands);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::changeChannelSelectedEvent ()
{
    QList<QSharedPointer<QUndoCommand> > commands;
    foreach (EventID event, GuiHelper::getSelectedEventIDs (currentEventView()))
    {
        QSharedPointer<EventManager> event_manager = currentVisModel()->getEventManager();
        ChannelID preselected_channel = event_manager->getEvent (event)->getChannel ();
        ChannelID new_channel = GuiHelper::selectChannel (preselected_channel,
                                                          currentVisModel());

        if (new_channel == preselected_channel)
            return;

        commands.append (QSharedPointer<QUndoCommand> (new ChangeChannelUndoCommand (event_manager,
                                                                             event,
                                                                             new_channel)));
    }
    executeCommands (commands);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::toAllChannelsSelectedEvent ()
{
    QList<QSharedPointer<QUndoCommand> > commands;
    foreach (EventID event, GuiHelper::getSelectedEventIDs (currentEventView()))
    {
        QSharedPointer<EventManager> event_manager = currentVisModel()->getEventManager();

        if (event_manager->getEvent (event)->getChannel () == UNDEFINED_CHANNEL)
            return;

        commands.append (QSharedPointer<QUndoCommand> (new ChangeChannelUndoCommand (event_manager,
                                                                             event,
                                                                             UNDEFINED_CHANNEL)));
    }
    executeCommands (commands);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::copyToChannelsSelectedEvent ()
{
    QList<QSharedPointer<QUndoCommand> > commands;
    foreach (EventID event_id, GuiHelper::getSelectedEventIDs (currentEventView()))
    {
        QSharedPointer<EventManager> event_manager = currentVisModel()->getEventManager();
        QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
        std::set<ChannelID> channels = GuiHelper::selectShownChannels (event->getChannel (),
                                                                       currentVisModel());

        if (channels.size () == 0)
            return;

        for (std::set<ChannelID>::iterator channel_iter = channels.begin();
        channel_iter != channels.end();
        ++channel_iter)
        {
            QSharedPointer<SignalEvent> new_event (new SignalEvent (*event));
            new_event->setChannel (*channel_iter);
            QSharedPointer<QUndoCommand> command (new NewEventUndoCommand (event_manager, new_event, 1));
            commands.append (command);
        }
    }
    executeCommands (commands);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::insertEventOverSelectedEvent ()
{
    QList<QSharedPointer<QUndoCommand> > commands;
    foreach (EventID event, GuiHelper::getSelectedEventIDs (currentEventView()))
    {
        if (event == UNDEFINED_EVENT_ID)
            return;

        QSharedPointer<EventManager> event_manager = currentVisModel()->getEventManager();
        QSharedPointer<SignalVisualisationModel> sv_model = currentVisModel();
        QSharedPointer<SignalEvent> new_event (new SignalEvent (*(event_manager->getEvent (event))));
        new_event->setType (sv_model->getActualEventCreationType ());

        commands.append (QSharedPointer<QUndoCommand>(new NewEventUndoCommand (event_manager,
                                                  new_event,
                                                  1)));
    }
    executeCommands (commands);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::evaluateEnabledness ()
{
    if (disableIfNoFileIsOpened (ACTIONS_()))
        return;
    if (disableIfNoEventsPossible (ACTIONS_()))
        return;

    bool event_one_channel_selected = (getTabSelectionState() == TAB_STATE_EVENT_SELECTED_ONE_CHANNEL);

    getQAction(COPY_TO_CHANNELS_())->setEnabled(event_one_channel_selected);
    getQAction(TO_ALL_CHANNEL_())->setEnabled(event_one_channel_selected);
    disableIfNoEventSelected (QStringList() << INSERT_OVER_() <<
                              CHANGE_CHANNEL_() << CHANGE_TYPE_() <<
                              DELETE_());
}


//-------------------------------------------------------------------------
void EventEditingGuiCommand::executeCommands (QList<QSharedPointer<QUndoCommand> > commands)
{
    MacroUndoCommand* macro = new MacroUndoCommand (commands);
    applicationContext()->getCurrentCommandExecuter()->executeCommand (macro);
}

}
