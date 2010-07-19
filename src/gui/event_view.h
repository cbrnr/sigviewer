#ifndef EVENT_VIEW_H
#define EVENT_VIEW_H

#include "base/sigviewer_user_types.h"

#include <set>

namespace BioSig_
{

class EventView
{
public:
    //-------------------------------------------------------------------------
    virtual void setShownEventTypes (std::set<EventType> const& event_types) = 0;

    //-------------------------------------------------------------------------
    virtual std::set<EventType> getShownEventTypes () const = 0;
};
}

#endif // EVENT_VIEW_H
