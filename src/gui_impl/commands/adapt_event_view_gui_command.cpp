// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "adapt_event_view_gui_command.h"
#include "gui_impl/gui_helper_functions.h"
#include "gui/signal_visualisation_model.h"

#include <QDebug>

namespace sigviewer
{

//-----------------------------------------------------------------------------
QString const AdaptEventViewGuiCommand::FIT_TO_EVENT_()
{
    static QString value = tr("Fit View to Selected Event");

    return value;
}

QString const AdaptEventViewGuiCommand::HIDE_EVENTS_OF_OTHER_TYPE_()
{
    static QString value = tr("Hide Events of other Type");

    return value;
}

QString const AdaptEventViewGuiCommand::SHOW_ALL_EVENTS_()
{
    static QString value = tr("Show all Events");

    return value;
}

QString const AdaptEventViewGuiCommand::GO_TO_NEXT_EVENT_()
{
    static QString value = tr("Goto and Select Next Event");

    return value;
}

QString const AdaptEventViewGuiCommand::GO_TO_PREVIOUS_EVENT_()
{
    static QString value = tr("Goto and Select Previous Event");

    return value;
}

QString const AdaptEventViewGuiCommand::SET_SHOWN_EVENTS_()
{
    static QString value = tr("Events...");

    return value;
}

QStringList const AdaptEventViewGuiCommand::ACTIONS_()
{
    static QStringList result = {
        AdaptEventViewGuiCommand::FIT_TO_EVENT_(),
        AdaptEventViewGuiCommand::GO_TO_NEXT_EVENT_(),
        AdaptEventViewGuiCommand::GO_TO_PREVIOUS_EVENT_(),
        AdaptEventViewGuiCommand::HIDE_EVENTS_OF_OTHER_TYPE_(),
        AdaptEventViewGuiCommand::SHOW_ALL_EVENTS_(),
        AdaptEventViewGuiCommand::SET_SHOWN_EVENTS_(),
    };

    return result;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator AdaptEventViewGuiCommand::registrator_ ("Adapt Event View",
    QSharedPointer<AdaptEventViewGuiCommand> (new AdaptEventViewGuiCommand));


//-----------------------------------------------------------------------------
AdaptEventViewGuiCommand::AdaptEventViewGuiCommand () :
    GuiActionCommand (ACTIONS_()),
    current_event_ (UNDEFINED_EVENT_ID),
    next_event_ (UNDEFINED_EVENT_ID),
    previous_event_ (UNDEFINED_EVENT_ID)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void AdaptEventViewGuiCommand::init ()
{
    setShortcut (GO_TO_NEXT_EVENT_(), QKeySequence("Ctrl+Right"));
    setShortcut (GO_TO_PREVIOUS_EVENT_(), QKeySequence("Ctrl+Left"));
    setShortcut (SET_SHOWN_EVENTS_(), tr("Ctrl+E"));

    setIcon (GO_TO_NEXT_EVENT_(), QIcon(":/images/ic_chevron_right_black_24dp.png"));
    setIcon (GO_TO_PREVIOUS_EVENT_(), QIcon(":/images/ic_chevron_left_black_24dp.png"));
    setIcon (SET_SHOWN_EVENTS_(), QIcon(":/images/ic_flag_black_24dp.png"));
    setIcon (FIT_TO_EVENT_(), QIcon(":/images/ic_zoom_out_map_black_24dp.png"));

    resetActionTriggerSlot (HIDE_EVENTS_OF_OTHER_TYPE_(), SLOT(hideEventsOfOtherType()));
    resetActionTriggerSlot (SHOW_ALL_EVENTS_(), SLOT(showAllEvents()));
    resetActionTriggerSlot (SET_SHOWN_EVENTS_(), SLOT(setShownEvents()));
}

//-----------------------------------------------------------------------------
void AdaptEventViewGuiCommand::trigger (QString const& action_name)
{
    if (action_name == FIT_TO_EVENT_())
        fitViewToEvent ();
    else if (action_name == GO_TO_NEXT_EVENT_())
        gotoAndSelectEvent (true);
    else if (action_name == GO_TO_PREVIOUS_EVENT_())
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
                                                                       applicationContext()->getEventColorManager());
    if (shown_types != new_shown_types)
        currentVisModel()->setShownEventTypes (new_shown_types);
    currentVisModel()->update ();
}


//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::evaluateEnabledness ()
{
    if (disableIfNoFileIsOpened (ACTIONS_()))
        return;
    if (disableIfNoEventsPossible (ACTIONS_()))
        return;

    disableIfNoEventSelected (QStringList() << HIDE_EVENTS_OF_OTHER_TYPE_()
                              << FIT_TO_EVENT_());

    setNextAndPreviousEvent ();
    getQAction (SET_SHOWN_EVENTS_())->setEnabled (currentEventView()->eventsHidable());
    getQAction (HIDE_EVENTS_OF_OTHER_TYPE_())->setEnabled (currentEventView()->eventsHidable());
    getQAction (SHOW_ALL_EVENTS_())->setEnabled (currentEventView()->eventsHidable());
    getQAction (FIT_TO_EVENT_())->setEnabled (currentEventView()->eventsHidable());
    getQAction (GO_TO_NEXT_EVENT_())->setEnabled (currentEventView()->eventsSlidable());
    getQAction (GO_TO_PREVIOUS_EVENT_())->setEnabled (currentEventView()->eventsSlidable());
}


//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::fitViewToEvent ()
{
    QList<QSharedPointer<SignalEvent const> > events = GuiHelper::getSelectedEvents (currentVisModel());
    if (events.size() != 1)
        return;

    QSharedPointer<SignalEvent const> event = events.first();

    float64 width = currentVisModel()->view()->getViewportWidth();
    float64 desired_pixel_per_sample = width / event->getDuration ();

    currentSignalViewSettings()->setPixelsPerSample (desired_pixel_per_sample);
    currentVisModel()->goToSample (event->getPosition ());
}

//-------------------------------------------------------------------------
void AdaptEventViewGuiCommand::gotoAndSelectEvent (bool forward)
{
    QSharedPointer<EventManager const> event_manager = currentVisModel()->getEventManager();
    if (event_manager.isNull())
    {
        qCritical() << tr("AdaptEventViewGuiCommand::gotoAndSelectEvent no EventManager available!!");
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
                                   (unsigned) event->getPosition());
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

    getQAction (GO_TO_NEXT_EVENT_())->setEnabled (next_event_ != UNDEFINED_EVENT_ID);
    getQAction (GO_TO_PREVIOUS_EVENT_())->setEnabled (previous_event_ != UNDEFINED_EVENT_ID);
}

}
