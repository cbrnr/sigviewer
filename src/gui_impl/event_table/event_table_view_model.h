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


#ifndef EVENT_TABLE_VIEW_MODEL_H
#define EVENT_TABLE_VIEW_MODEL_H

#include "gui/event_view.h"

namespace SigViewer_
{

class EventTableWidget;

//-----------------------------------------------------------------------------
class EventTableViewModel : public EventView
{
public:
    //-------------------------------------------------------------------------
    EventTableViewModel (EventTableWidget& event_table_widget);

    //-------------------------------------------------------------------------
    virtual void setShownEventTypes (std::set<EventType> const& /*event_types*/) {}

    //-------------------------------------------------------------------------
    virtual std::set<EventType> getShownEventTypes () const {return std::set<EventType> ();}

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager const> getEventManager () const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager> getEventManager ();

    //-------------------------------------------------------------------------
    virtual QList<EventID> getSelectedEvents () const;

private:
    EventTableWidget& event_table_widget_;
};

}

#endif // EVENT_TABLE_VIEW_MODEL_H
