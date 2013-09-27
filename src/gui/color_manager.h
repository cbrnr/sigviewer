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


#ifndef EVENT_COLOR_MANAGER
#define EVENT_COLOR_MANAGER

#include "base/sigviewer_user_types.h"
#include "file_handling_impl/event_table_file_reader.h"

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

    void saveSettings();

    QColor getChannelColor (ChannelID channel_id) const;
    QColor getDefaultChannelColor () const;
    void setDefaultChannelColor (QColor const& color);
    void setChannelColor (ChannelID channel_id, QColor const& color);

    QColor getEventColor (EventType type) const;
    QColor getDefaultEventColor (EventType type = UNDEFINED_EVENT_TYPE) const;
    void setEventColor (EventType type, const QColor& color);

    static bool isDark (QColor const& color);
private:
    Q_DISABLE_COPY(ColorManager)

    void loadSettings();
    void loadDefaultEventColors ();

    static const char* DEFAULT_CHANNEL_COLOR_SETTING_;
    QColor default_channel_color_;
    QColor default_event_color_;

    typedef QMap<EventType, QColor> EventColorMap;
    typedef QMap<ChannelID, QColor> ChannelColorMap;

    EventColorMap event_type2color_;
    EventColorMap default_event_colors_;
    ChannelColorMap channel_color_map_;
    EventTableFileReader event_table_file_reader_;
};

} // namespace SigViewer_

#endif
