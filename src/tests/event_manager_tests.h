// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef EVENT_MANAGER_TESTS_H
#define EVENT_MANAGER_TESTS_H

#include "test.h"

namespace sigviewer
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
