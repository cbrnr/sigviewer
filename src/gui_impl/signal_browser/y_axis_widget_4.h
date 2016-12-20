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


#ifndef Y_AXIS_GRAPHICS_ITEM_H
#define Y_AXIS_GRAPHICS_ITEM_H

#include "base/sigviewer_user_types.h"

#include <QWidget>
#include <QMap>

namespace sigviewer
{

class SignalGraphicsItem;

class YAxisWidget : public QWidget
{
    Q_OBJECT
public:
    YAxisWidget(QWidget* parent);

    virtual ~YAxisWidget () {}

    void addChannel(ChannelID channel_nr, SignalGraphicsItem const* const signal_item);
    void removeChannel(ChannelID channel_nr);

public slots:
    void changeSignalHeight (unsigned signal_height);
    void changeYStart (int32 y_start);
    void updateChannel (ChannelID channel_id);

private:
    virtual void paintEvent (QPaintEvent* event = 0);
    virtual void contextMenuEvent (QContextMenuEvent* event);

    void paintYAxisLabels (QPainter* painter, float64 offset,
                           float64 y_grid_pixel_intervall,
                           double value_range_fragment,
                           QString const& unit_string);

    void paintYUnits (QPainter* painter, QString const& unit_string);

    unsigned channel_height_;
    int32 y_start_;
    QMap<ChannelID, SignalGraphicsItem const*> channel_nr2signal_graphics_item_;
};

}

#endif // Y_AXIS_GRAPHICS_ITEM_H
