#ifndef EVENT_VIEW_H
#define EVENT_VIEW_H

#include "base/sigviewer_user_types.h"

namespace BioSig_
{

class EventView
{
public:
    virtual void setShownEventTypes () = 0;
    std::set<EventType> getShownEventTypes () = 0;
};
}

#endif // EVENT_VIEW_H
