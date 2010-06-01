#ifndef Y_AXIS_GRAPHICS_ITEM_H
#define Y_AXIS_GRAPHICS_ITEM_H

#include "../base/user_types.h"

#include <QWidget>
#include <QMap>

namespace BioSig_
{

class SignalGraphicsItem;

class YAxisWidget : public QWidget
{
    Q_OBJECT
public:
    YAxisWidget(QWidget* parent);

    virtual ~YAxisWidget () {}
    virtual QSize sizeHint () const;

    void addChannel(int32 channel_nr, SignalGraphicsItem const* const signal_item);
    void removeChannel(int32 channel_nr);

public slots:
    void changeSignalHeight (uint32 signal_height);
    void changeYStart (int32 y_start);
    void updateChannel (ChannelID channel_id);

private:
    virtual void paintEvent (QPaintEvent* event = 0);
    virtual void contextMenuEvent (QContextMenuEvent* event);

    void paintYAxisLabels (QPainter* painter, float64 offset,
                              float64 y_zoom, float64 y_grid_pixel_intervall);

    unsigned signal_height_;
    int32 y_start_;
    QMap<int32, SignalGraphicsItem const*> channel_nr2signal_graphics_item_;
};

}

#endif // Y_AXIS_GRAPHICS_ITEM_H
