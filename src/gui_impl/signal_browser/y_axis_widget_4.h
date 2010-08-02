#ifndef Y_AXIS_GRAPHICS_ITEM_H
#define Y_AXIS_GRAPHICS_ITEM_H

#include "base/sigviewer_user_types.h"

#include <QWidget>
#include <QMap>

namespace SigViewer_
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
