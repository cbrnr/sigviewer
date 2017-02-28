// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef Y_AXIS_GRAPHICS_ITEM_H
#define Y_AXIS_GRAPHICS_ITEM_H

#include "base/sigviewer_user_types.h"
#include "gui/signal_view_settings.h"


#include <QWidget>
#include <QMap>

namespace sigviewer
{

class SignalGraphicsItem;

class YAxisWidget : public QWidget
{
    Q_OBJECT
public:
    YAxisWidget(QWidget* parent, QSharedPointer<SignalViewSettings const> signal_view_settings);

    virtual ~YAxisWidget () {}

    void addChannel(ChannelID channel_nr, SignalGraphicsItem const* const signal_item);
    void removeChannel(ChannelID channel_nr);

public slots:
    void changeSignalHeight (unsigned signal_height);
    void changeYStart (int32 y_start);
    void updateChannel (ChannelID channel_id);
    void enableSeparator(bool enable);
    void changeLabelColor(QColor labelColor);

private:
    virtual void paintEvent (QPaintEvent* event = 0);
    virtual void contextMenuEvent (QContextMenuEvent* event);

    void paintYAxisLabels (QPainter* painter, float64 offset,
                           float64 y_grid_pixel_intervall,
                           double value_range_fragment,
                           QString const& unit_string, float64 channelHeight);

    void paintYUnits (QPainter* painter, QString const& unit_string,
                      float64 channelHeight);

    unsigned channel_height_;
    int32 y_start_;
    QMap<ChannelID, SignalGraphicsItem const*> channel_nr2signal_graphics_item_;
    QSharedPointer<SignalViewSettings const> signal_view_settings_;
    bool enable_separator;
    QColor label_color_;

};

}

#endif // Y_AXIS_GRAPHICS_ITEM_H
