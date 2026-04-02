// © SigViewer developers
//
// License: GPL-3.0

#ifndef EVENT_TABLE_VIEW_MODEL_H
#define EVENT_TABLE_VIEW_MODEL_H

#include "gui/event_view.h"

namespace sigviewer {

class EventTableWidget;

class EventTableViewModel : public EventView {
   public:
    EventTableViewModel(EventTableWidget& event_table_widget);

    virtual void setShownEventTypes(std::set<EventType> const& /*event_types*/) {}

    virtual std::set<EventType> getShownEventTypes() const {
        return std::set<EventType>();
    }

    virtual QSharedPointer<EventManager const> getEventManager() const;

    virtual QSharedPointer<EventManager> getEventManager();

    virtual QList<EventID> getSelectedEvents() const;

   private:
    EventTableWidget& event_table_widget_;
};

}  // namespace sigviewer

#endif  // EVENT_TABLE_VIEW_MODEL_H
