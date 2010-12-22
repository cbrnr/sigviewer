#include "event_table_view_model.h"
#include "event_table_widget.h"


namespace SigViewer_
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
