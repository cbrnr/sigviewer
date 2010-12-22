#include "adapt_event_view_gui_command.h"
#include "gui_impl/gui_helper_functions.h"
#include "gui/signal_visualisation_model.h"

#include <QDebug>

namespace SigViewer_
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
AdaptEventViewGuiCommand::AdaptEventViewGuiCommand () :
    GuiActionCommand (ACTIONS_),
    current_event_ (UNDEFINED_EVENT_ID),
    next_event_ (UNDEFINED_EVENT_ID),
    previous_event_ (UNDEFINED_EVENT_ID)
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
    setIcon (FIT_TO_EVENT_, QIcon(":/images/icons/zoom_fit_event_horizontal.png"));

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
    QList<QSharedPointer<SignalEvent const> > events = GuiHelper::getSelectedEvents (currentEventView());

    if (events.size() != 1)
        return;

    std::set<EventType> shown_types;
    shown_types.insert (events.first()->getType ());
    currentVisModel()->setShownEventTypes (shown_types);
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::showAllEvents ()
{
    std::set<EventType> shown_types = currentVisModel()->getEventManager()->getEventTypes();
    currentVisModel()->setShownEventTypes (shown_types);
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::setShownEvents ()
{
    std::set<EventType> shown_types = currentVisModel()->getShownEventTypes ();
    std::set<EventType> new_shown_types = GuiHelper::selectEventTypes (shown_types,
                                                                       currentVisModel()->getEventManager(),
                                                                       applicationContext()->getEventColorManager(),
                                                                       true);
    if (shown_types != new_shown_types)
        currentVisModel()->setShownEventTypes (new_shown_types);
    currentVisModel()->update ();
}


//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::evaluateEnabledness ()
{
    if (disableIfNoFileIsOpened (ACTIONS_))
        return;
    if (disableIfNoEventsPossible (ACTIONS_))
        return;

    disableIfNoEventSelected (QStringList() << HIDE_EVENTS_OF_OTHER_TYPE_
                              << FIT_TO_EVENT_);

    setNextAndPreviousEvent ();
    getQAction (SET_SHOWN_EVENTS_)->setEnabled (currentEventView()->eventsHidable());
    getQAction (HIDE_EVENTS_OF_OTHER_TYPE_)->setEnabled (currentEventView()->eventsHidable());
    getQAction (SHOW_ALL_EVENTS_)->setEnabled (currentEventView()->eventsHidable());
    getQAction (FIT_TO_EVENT_)->setEnabled (currentEventView()->eventsHidable());
    getQAction (GO_TO_NEXT_EVENT_)->setEnabled (currentEventView()->eventsSlidable());
    getQAction (GO_TO_PREVIOUS_EVENT_)->setEnabled (currentEventView()->eventsSlidable());
}


//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::fitViewToEvent ()
{
    QList<QSharedPointer<SignalEvent const> > events = GuiHelper::getSelectedEvents (currentVisModel());
    if (events.size() != 1)
        return;

    QSharedPointer<SignalEvent const> event = events.first();

    float32 width = currentVisModel()->view()->getViewportWidth();
    float32 desired_pixel_per_sample = width / event->getDuration ();

    currentSignalViewSettings()->setPixelsPerSample (desired_pixel_per_sample);
    currentVisModel()->goToSample (event->getPosition ());
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::gotoAndSelectEvent (bool forward)
{
    QSharedPointer<EventManager const> event_manager = currentVisModel()->getEventManager();
    if (event_manager.isNull())
    {
        qCritical() << "AdaptEventViewGuiCommand::gotoAndSelectEvent no EventManager available!!";
        return;
    }

    EventID event_id;
    if (forward)
        event_id = next_event_;
    else
        event_id = previous_event_;

    QSharedPointer<SignalEvent const> event = event_manager->getEvent (event_id);
    if (!event.isNull())
    {
        GuiHelper::animateProperty(currentVisModel().data(), "sample_position_",
                                   currentVisModel()->getShownPosition(),
                                   event->getPosition());
        currentVisModel()->selectEvent (event->getId());
    }
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::setNextAndPreviousEvent ()
{
    QList<QSharedPointer<SignalEvent const> > events = GuiHelper::getSelectedEvents (currentVisModel());
    if (events.size() != 1)
        return;

    QSharedPointer<SignalEvent const> event = events.first();
    if (!event.isNull ())
    {
        if (current_event_ == event->getId ())
            return;
        current_event_ = event->getId ();
        QSharedPointer<EventManager const> event_manager = currentVisModel()->getEventManager ();
        next_event_ = event_manager->getNextEventOfSameType (event->getId ());
        previous_event_ = event_manager->getPreviousEventOfSameType (event->getId ());
        qDebug () << "next event " << next_event_;
        qDebug () << "previous event "<< previous_event_;
    }

    getQAction (GO_TO_NEXT_EVENT_)->setEnabled (next_event_ != UNDEFINED_EVENT_ID);
    getQAction (GO_TO_PREVIOUS_EVENT_)->setEnabled (previous_event_ != UNDEFINED_EVENT_ID);
}


}
