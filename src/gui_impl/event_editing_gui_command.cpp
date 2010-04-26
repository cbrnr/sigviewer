#include "event_editing_gui_command.h"
#include "gui_helper_functions.h"
#include "../application_context.h"
#include "../editing_commands/delete_event_undo_command.h"
#include "../editing_commands/change_type_undo_command.h"
#include "../editing_commands/change_channel_undo_command.h"
#include "../editing_commands/new_event_undo_command.h"
#include "../editing_commands/macro_undo_command.h"

#include <iostream>

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const EventEditingGuiCommand::DELETE_ = "Delete";
QString const EventEditingGuiCommand::CHANGE_TYPE_ = "Change Type...";
QString const EventEditingGuiCommand::CHANGE_CHANNEL_ = "Change Channel...";
QString const EventEditingGuiCommand::TO_ALL_CHANNEL_ = "To all Channels";
QString const EventEditingGuiCommand::COPY_TO_CHANNELS_ = "Copy to Channels...";
QString const EventEditingGuiCommand::INSERT_OVER_ = "Insert Over";
QStringList const EventEditingGuiCommand::ACTIONS_ = QStringList () <<
                                                     EventEditingGuiCommand::DELETE_ <<
                                                     EventEditingGuiCommand::CHANGE_TYPE_ <<
                                                     EventEditingGuiCommand::CHANGE_CHANNEL_ <<
                                                     EventEditingGuiCommand::TO_ALL_CHANNEL_ <<
                                                     EventEditingGuiCommand::COPY_TO_CHANNELS_ <<
                                                     EventEditingGuiCommand::INSERT_OVER_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator EventEditingGuiCommand::registrator_ ("Event Editing",
                                                                  QSharedPointer<EventEditingGuiCommand> (new EventEditingGuiCommand));


//-----------------------------------------------------------------------------
EventEditingGuiCommand::EventEditingGuiCommand ()
    : GuiActionCommand (ACTIONS_)
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
    resetActionTriggerSlot (DELETE_, SLOT (deleteSelectedEvent()));
    resetActionTriggerSlot (CHANGE_TYPE_, SLOT (changeTypeSelectedEvent()));
    resetActionTriggerSlot (CHANGE_CHANNEL_, SLOT (changeChannelSelectedEvent()));
    resetActionTriggerSlot (TO_ALL_CHANNEL_, SLOT (toAllChannelsSelectedEvent()));
    resetActionTriggerSlot (COPY_TO_CHANNELS_, SLOT (copyToChannelsSelectedEvent()));
    resetActionTriggerSlot (INSERT_OVER_, SLOT (insertEventOverSelectedEvent()));

    getQAction (DELETE_)->setIcon (QIcon(":/images/icons/editdelete.png"));
    getQAction (CHANGE_TYPE_)->setIcon (QIcon (":/images/change_type_22x22.png"));
    getQAction (CHANGE_CHANNEL_)->setIcon (QIcon (":/images/change_channel_22x22.png"));
    getQAction (TO_ALL_CHANNEL_)->setIcon (QIcon (":/images/to_all_channels_22x22.png"));
    getQAction (COPY_TO_CHANNELS_)->setIcon (QIcon (":/images/copy_to_channels_22x22.png"));
    getQAction (INSERT_OVER_)->setIcon (QIcon (":/images/icons/add.png"));
    getQAction (DELETE_)->setShortcut (QKeySequence::Delete);
    getQAction (INSERT_OVER_)->setShortcut (QKeySequence("Ctrl+I"));
}

//-----------------------------------------------------------------------------
void EventEditingGuiCommand::deleteSelectedEvent ()
{
    EventID event = GuiHelper::getSelectedEventID ();
    if (event == UNDEFINED_EVENT_ID)
        return;

    QUndoCommand* command = new DeleteEventUndoCommand (
            ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager(),
            event);
    executeCommand (command);
}

//-----------------------------------------------------------------------------
void EventEditingGuiCommand::changeTypeSelectedEvent ()
{
    EventID event = GuiHelper::getSelectedEventID ();
    if (event == UNDEFINED_EVENT_ID)
        return;

    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    EventType pre_selected_type = event_manager->getEvent (event)->getType ();
    EventType new_type = GuiHelper::selectEventType (pre_selected_type);

    if (new_type == UNDEFINED_EVENT_TYPE)
        return;

    QUndoCommand* command = new ChangeTypeUndoCommand (
            event_manager,
            event,
            new_type);
    executeCommand (command);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::changeChannelSelectedEvent ()
{
    EventID event = GuiHelper::getSelectedEventID ();
    if (event == UNDEFINED_EVENT_ID)
        return;

    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    ChannelID preselected_channel = event_manager->getEvent (event)->getChannel ();
    ChannelID new_channel = GuiHelper::selectChannel (preselected_channel);

    if (new_channel == preselected_channel)
        return;

    QUndoCommand* change_channel_command = new ChangeChannelUndoCommand (event_manager,
                                                                       event,
                                                                       new_channel);
    executeCommand (change_channel_command);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::toAllChannelsSelectedEvent ()
{
    EventID event = GuiHelper::getSelectedEventID ();
    if (event == UNDEFINED_EVENT_ID)
        return;

    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();

    if (event_manager->getEvent (event)->getChannel () == UNDEFINED_CHANNEL)
        return;

    QUndoCommand* change_channel_command = new ChangeChannelUndoCommand (event_manager,
                                                                       event,
                                                                       UNDEFINED_CHANNEL);
    executeCommand (change_channel_command);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::copyToChannelsSelectedEvent ()
{
    EventID event_id = GuiHelper::getSelectedEventID ();
    if (event_id == UNDEFINED_EVENT_ID)
        return;

    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
    std::set<ChannelID> channels = GuiHelper::selectShownChannels (event->getChannel ());

    if (channels.size () == 0)
        return;

    QList<QSharedPointer<QUndoCommand> > new_commands;
    for (std::set<ChannelID>::iterator channel_iter = channels.begin();
         channel_iter != channels.end();
         ++channel_iter)
    {
        QSharedPointer<SignalEvent> new_event (new SignalEvent (*event));
        new_event->setChannel (*channel_iter);
        QSharedPointer<QUndoCommand> command (new NewEventUndoCommand (event_manager, new_event, 1));
        new_commands.append (command);
    }
    QUndoCommand* macro = new MacroUndoCommand (new_commands);
    executeCommand (macro);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::insertEventOverSelectedEvent ()
{
    EventID event = GuiHelper::getSelectedEventID ();
    if (event == UNDEFINED_EVENT_ID)
        return;

    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();
    QSharedPointer<MainWindowModel> mw_model = ApplicationContext::getInstance()->getMainWindowModel ();
    QSharedPointer<SignalVisualisationModel> sv_model = mw_model->getCurrentSignalVisualisationModel ();
    QSharedPointer<SignalEvent> new_event (new SignalEvent (*(event_manager->getEvent (event))));
    new_event->setType (sv_model->getActualEventCreationType ());

    QUndoCommand* new_event_command = new NewEventUndoCommand (event_manager,
                                                             new_event,
                                                             1);
    executeCommand (new_event_command);
}

//-------------------------------------------------------------------------
void EventEditingGuiCommand::evaluateEnabledness ()
{
    if (getApplicationState () == APP_STATE_FILE_OPEN)
    {
        if (getTabSelectionState () == TAB_STATE_NO_EVENT_SELECTED)
            emit qActionEnabledChanged (false);
        else
        {
            emit qActionEnabledChanged (true);
            if (getTabSelectionState () == TAB_STATE_EVENT_SELECTED_ALL_CHANNELS)
            {
                getQAction (TO_ALL_CHANNEL_)->setEnabled (false);
                getQAction (COPY_TO_CHANNELS_)->setEnabled (false);
            }
        }
    }
    else
        emit qActionEnabledChanged (false);
}


//-------------------------------------------------------------------------
void EventEditingGuiCommand::executeCommand (QUndoCommand* command)
{
    ApplicationContext::getInstance()->getCurrentCommandExecuter()->executeCommand (command);
}



}
