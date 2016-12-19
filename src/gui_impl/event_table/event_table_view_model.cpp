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


#include "event_table_view_model.h"
#include "event_table_widget.h"


namespace sigviewer
{

//-----------------------------------------------------------------------------
EventTableViewModel::EventTableViewModel (EventTableWidget& event_table_widget)
    : EventView (false, false),
      event_table_widget_ (event_table_widget)
{

}

//-----------------------------------------------------------------------------
QSharedPointer<EventManager const> EventTableViewModel::getEventManager () const
{
    return event_table_widget_.getEventManager ();
}

//-----------------------------------------------------------------------------
QSharedPointer<EventManager> EventTableViewModel::getEventManager ()
{
    return event_table_widget_.getEventManager ();
}

//-----------------------------------------------------------------------------
QList<EventID> EventTableViewModel::getSelectedEvents () const
{
    return event_table_widget_.getSelectedEvents ();
}




}
