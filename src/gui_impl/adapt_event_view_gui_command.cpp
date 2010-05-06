#include "adapt_event_view_gui_command.h"
#include "gui_helper_functions.h"
#include "../application_context.h"
#include "../gui/signal_visualisation_model.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString const AdaptEventViewGuiCommand::FIT_TO_EVENT_ = "Fit View to Selected Event";
QString const AdaptEventViewGuiCommand::HIDE_EVENTS_OF_OTHER_TYPE_ = "Hide Events of other Type";
QString const AdaptEventViewGuiCommand::SHOW_ALL_EVENTS_ = "Show all Events";
QString const AdaptEventViewGuiCommand::GO_TO_NEXT_EVENT_ = "Goto and Select Next Event";
QString const AdaptEventViewGuiCommand::GO_TO_PREVIOUS_EVENT_ = "Goto and Select Previous Event";
QString const AdaptEventViewGuiCommand::SET_SHOWN_EVENTS_ = "Events...";
QStringList const AdaptEventViewGuiCommand::ACTIONS_ = QStringList() <<
                                              AdaptEventViewGuiCommand::FIT_TO_EVENT_ <<
                                              AdaptEventViewGuiCommand::GO_TO_NEXT_EVENT_ <<
                                              AdaptEventViewGuiCommand::GO_TO_PREVIOUS_EVENT_ <<
                                              AdaptEventViewGuiCommand::HIDE_EVENTS_OF_OTHER_TYPE_ <<
                                              AdaptEventViewGuiCommand::SHOW_ALL_EVENTS_ <<
                                              AdaptEventViewGuiCommand::SET_SHOWN_EVENTS_;

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator AdaptEventViewGuiCommand::registrator_ ("Adapt Event View",
    QSharedPointer<AdaptEventViewGuiCommand> (new AdaptEventViewGuiCommand));


//-----------------------------------------------------------------------------
AdaptEventViewGuiCommand::AdaptEventViewGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void AdaptEventViewGuiCommand::init ()
{
    setShortcut (GO_TO_NEXT_EVENT_, QKeySequence("Ctrl+Right"));
    setShortcut (GO_TO_PREVIOUS_EVENT_, QKeySequence("Ctrl+Left"));

    setIcon (GO_TO_NEXT_EVENT_, QIcon(":/images/icons/next.png"));
    setIcon (GO_TO_PREVIOUS_EVENT_, QIcon(":/images/icons/previous.png"));
    setIcon (SET_SHOWN_EVENTS_, QIcon(":/images/events_22x22.png"));

    resetActionTriggerSlot (HIDE_EVENTS_OF_OTHER_TYPE_, SLOT(hideEventsOfOtherType()));
    resetActionTriggerSlot (SHOW_ALL_EVENTS_, SLOT(showAllEvents()));
    resetActionTriggerSlot (SET_SHOWN_EVENTS_, SLOT(setShownEvents()));
}

//-----------------------------------------------------------------------------
void AdaptEventViewGuiCommand::trigger (QString const& action_name)
{
    if (action_name == FIT_TO_EVENT_)
        fitViewToEvent ();
    else if (action_name == GO_TO_NEXT_EVENT_)
        gotoAndSelectEvent (true);
    else if (action_name == GO_TO_PREVIOUS_EVENT_)
        gotoAndSelectEvent (false);
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::hideEventsOfOtherType ()
{
    QSharedPointer<SignalEvent const> event = GuiHelper::getSelectedEvent ();

    std::set<EventType> shown_types;
    shown_types.insert (event->getType ());
    currentVisModel()->setShownEventTypes (shown_types);
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::showAllEvents ()
{
    std::set<EventType> shown_types = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager()->getAllPossibleEventTypes ();
    currentVisModel()->setShownEventTypes (shown_types);
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::setShownEvents ()
{
    std::set<EventType> shown_types = currentVisModel()->getShownEventTypes ();
    std::set<EventType> new_shown_types = GuiHelper::selectEventTypes (shown_types);
    if (shown_types != new_shown_types)
        currentVisModel()->setShownEventTypes (new_shown_types);
}


//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::evaluateEnabledness ()
{
    disableIfNoFileIsOpened (ACTIONS_);
    disableIfNoEventSelected (QStringList() << HIDE_EVENTS_OF_OTHER_TYPE_
                              << GO_TO_PREVIOUS_EVENT_
                              << GO_TO_NEXT_EVENT_
                              << FIT_TO_EVENT_);
}


//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::fitViewToEvent ()
{
    QSharedPointer<SignalEvent const> event = GuiHelper::getSelectedEvent ();

    float32 width = currentVisModel()->getShownSignalWidth ();
    float32 desired_pixel_per_sample = width / event->getDuration ();
    float32 pixel_per_sample = desired_pixel_per_sample;

    currentVisModel()->setPixelPerSample (pixel_per_sample);
    currentVisModel()->updateLayout ();
    currentVisModel()->goToSample (event->getPosition ());
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::gotoAndSelectEvent (bool forward)
{
    QSharedPointer<EventManager> event_manager = ApplicationContext::getInstance()->getCurrentFileContext()->getEventManager();

    QSharedPointer<SignalEvent const> event = GuiHelper::getSelectedEvent();

    QMap<uint32, EventID> events = event_manager->getEventPositions (event->getType());
    QMap<uint32, EventID>::iterator current_event_iter = events.find (event->getPosition());
    if (forward)
        ++current_event_iter;
    else if (current_event_iter != events.begin())
        --current_event_iter;
    else
        return;

    if (current_event_iter != events.end())
    {
        currentVisModel()->goToSample (current_event_iter.key ());
        currentVisModel()->selectEvent (current_event_iter.value ());
    }
}

}
