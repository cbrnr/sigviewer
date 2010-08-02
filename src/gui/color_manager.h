#ifndef EVENT_COLOR_MANAGER
#define EVENT_COLOR_MANAGER

#include "base/sigviewer_user_types.h"
#include "../file_handling_impl/event_table_file_reader.h"

#include <QMap>
#include <QColor>
#include <QPixmap>

namespace SigViewer_
{

// color manager
class ColorManager
{
public:
    ColorManager ();
    ~ColorManager ();

    static QColor const DEFAULT_EVENT_COLOR_;

    void loadSettings();
    void saveSettings();

    QColor getChannelColor (ChannelID channel_id) const;
    QColor getDefaultChannelColor () const;
    void setDefaultChannelColor (QColor const& color);
    void setChannelColor (ChannelID channel_id, QColor const& color);

    QColor getEventColor (EventType type) const;
    void setEventColor (EventType type, const QColor& color);

    static bool isDark (QColor const& color);
private:
    static const char* DEFAULT_CHANNEL_COLOR_SETTING_;
    QColor default_channel_color_;

    typedef QMap<EventType, QColor> EventColorMap;
    typedef QMap<ChannelID, QColor> ChannelColorMap;

    // not allowed
    ColorManager(const ColorManager& src);
    const ColorManager& operator=(const ColorManager& src);

    EventColorMap event_type2color_;
    ChannelColorMap channel_color_map_;
    EventTableFileReader event_table_file_reader_;
};

} // namespace SigViewer_

#endif
