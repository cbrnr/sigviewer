#ifndef EVENT_TABLE_WIDGET_TESTS_H
#define EVENT_TABLE_WIDGET_TESTS_H

#include "test.h"

namespace SigViewer_
{
namespace Tests_
{

class EventTableWidgetTests : public Test
{
public:
    EventTableWidgetTests () : Test(tr("Event Table Tests")) {}

    virtual QString run ();
private:
    QString basicCreation (QSharedPointer<EventManager> event_manager);

    QString newEvents (QSharedPointer<EventManager> event_manager);

    QString deleteEvents (QSharedPointer<EventManager> event_manager);

    QString changedEvents (QSharedPointer<EventManager> event_manager);

};

}

}

#endif // EVENT_TABLE_WIDGET_TESTS_H
