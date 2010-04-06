// event_color_manager.h

#ifndef EVENT_COLOR_MANAGER
#define EVENT_COLOR_MANAGER

#include "base/user_types.h"

#include <QMap>
#include <QColor>

namespace BioSig_
{

class EventTableFileReader;

// event color manager
class EventColorManager
{
public:
    EventColorManager(EventTableFileReader& event_table_reader);

    void loadSettings();
    void saveSettings();

    const QColor& getEventColor (EventID id) const;
    void setEventColor (EventID id, const QColor& color);

private:
    typedef QMap<EventID, QColor> Int2ColorMap;

    // not allowed
    EventColorManager(const EventColorManager& src);
    const EventColorManager& operator=(const EventColorManager& src);

    Int2ColorMap event_type2color_;
    EventTableFileReader& event_table_file_reader_;
};

} // namespace BioSig_

#endif
