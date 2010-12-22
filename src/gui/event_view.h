#ifndef EVENT_VIEW_H
#define EVENT_VIEW_H

#include "base/sigviewer_user_types.h"
#include "file_handling/event_manager.h"

#include <QSharedPointer>

#include <set>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class EventView
{
public:
    //-------------------------------------------------------------------------
    EventView (bool events_hidable = true, bool events_slidable = true)
        : events_hidable_ (events_hidable),
          events_slidable_ (events_slidable)
    {}

    //-------------------------------------------------------------------------
    virtual ~EventView () {}

    //-------------------------------------------------------------------------
    virtual bool eventsHidable () const {return events_hidable_;}

    //-------------------------------------------------------------------------
    virtual bool eventsSlidable () const {return events_slidable_;}

    //-------------------------------------------------------------------------
    virtual void setShownEventTypes (std::set<EventType> const& event_types) = 0;

    //-------------------------------------------------------------------------
    virtual std::set<EventType> getShownEventTypes () const = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager const> getEventManager () const = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager> getEventManager () = 0;

    //-------------------------------------------------------------------------
    /// @return the id of the currently selected signal event
    virtual QList<EventID> getSelectedEvents () const = 0;

private:
    bool events_hidable_;
    bool events_slidable_;

};
}

#endif // EVENT_VIEW_H
