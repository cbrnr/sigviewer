// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "event_manager_tests.h"

namespace sigviewer
{

namespace Tests_
{

QString EventManagerTests::run ()
{
    QSharedPointer<EventManager> event_manager = createEventManagerWithDummyData();
    VERIFY (event_manager->getAllEvents().size() == 44, "event amount");
    foreach (EventID event, event_manager->getAllEvents())
    {
        VERIFY (event_manager->getEvent(event).isNull() == false, "event exists");
    }

    RUN_SUB_TEST(testEventCreation(event_manager));

    return "";
}

QString EventManagerTests::testEventCreation (QSharedPointer<EventManager> evt_mgr)
{
    QSharedPointer<SignalEvent const> event = evt_mgr->createEvent (1, 10, 100, 1, UNDEFINED_STREAM_ID);
    VERIFY (event.isNull() == false, "event created");
    VERIFY (event->getChannel() == 1, "event created channel");
    VERIFY (event->getDuration() == 100, "event created duration");
    VERIFY (event->getDurationInSec() == 100 / evt_mgr->getSampleRate(), "event created duration in sec");
    VERIFY (event->getPosition() == 10, "event created position");
    VERIFY (event->getType() == 1, "event created type");
    return "";
}

}

}
