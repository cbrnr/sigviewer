// event_color_manager.h

#ifndef EVENT_COLOR_MANAGER
#define EVENT_COLOR_MANAGER

#include "base/user_types.h"

#include <QMap>
#include <QColor>

namespace BioSig_
{

// event color manager
class EventColorManager
{
public:
    EventColorManager ();
    ~EventColorManager ();

    void loadSettings();
    void saveSettings();

    const QColor& getEventColor (EventType type) const;
    void setEventColor (EventType type, const QColor& color);

private:
    typedef QMap<EventType, QColor> EventColorMap;

    // not allowed
    EventColorManager(const EventColorManager& src);
    const EventColorManager& operator=(const EventColorManager& src);

    EventColorMap event_type2color_;
};

} // namespace BioSig_

#endif
