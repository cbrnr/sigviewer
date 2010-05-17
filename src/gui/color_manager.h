#ifndef EVENT_COLOR_MANAGER
#define EVENT_COLOR_MANAGER

#include "base/user_types.h"

#include <QMap>
#include <QColor>

namespace BioSig_
{

// color manager
class ColorManager
{
public:
    ColorManager ();
    ~ColorManager ();

    static QColor const DEFAULT_EVENT_COLOR_;
    static QColor const DEFAULT_CHANNEL_COLOR_;

    void loadSettings();
    void saveSettings();

    QColor getChannelColor (ChannelID channel_id) const;
    void setChannelColor (ChannelID channel_id, QColor const& color);

    QColor getEventColor (EventType type) const;
    void setEventColor (EventType type, const QColor& color);

    static bool isDark (QColor const& color);
private:
    typedef QMap<EventType, QColor> EventColorMap;
    typedef QMap<ChannelID, QColor> ChannelColorMap;

    // not allowed
    ColorManager(const ColorManager& src);
    const ColorManager& operator=(const ColorManager& src);

    EventColorMap event_type2color_;
    ChannelColorMap channel_color_map_;
};

} // namespace BioSig_

#endif
