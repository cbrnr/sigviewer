// event_color_manager.h

#ifndef EVENT_COLOR_MANAGER
#define EVENT_COLOR_MANAGER

#include "base/user_types.h"

#include <QMap>
#include <QColor>

namespace BioSig_
{

class EventTableFileReader;
class SettingsManager;

// event color manager
class EventColorManager
{
public:
    EventColorManager(EventTableFileReader& event_table_reader);

    void loadSettings();
    void saveSettings();

    const QColor& getEventColor(int32 id) const;
    void setEventColor(int32 id, const QColor& color);

private:
    typedef QMap<int32, QColor> Int2ColorMap;

    // not allowed
    EventColorManager(const EventColorManager& src);
    const EventColorManager& operator=(const EventColorManager& src);

    Int2ColorMap event_type2color_;
    EventTableFileReader& event_table_file_reader_;
};

} // namespace BioSig_

#endif
