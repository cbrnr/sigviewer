// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef EVENT_VIEW_H
#define EVENT_VIEW_H

#include "base/sigviewer_user_types.h"
#include "file_handling/event_manager.h"

#include <QSharedPointer>

#include <set>

namespace sigviewer
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
