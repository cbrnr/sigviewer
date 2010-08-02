#ifndef EVENT_MANAGER_TESTS_H
#define EVENT_MANAGER_TESTS_H

#include "test.h"

namespace SigViewer_
{
namespace Tests_
{
class EventManagerTests : public Test
{
public:
    EventManagerTests () : Test(tr("EventManager Tests")) {}

    virtual QString run ();
private:
    QString testEventCreation (QSharedPointer<EventManager> evt_mgr);
};
}
}

#endif // EVENT_MANAGER_TESTS_H
