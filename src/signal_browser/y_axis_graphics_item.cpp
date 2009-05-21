#include "y_axis_graphics_item.h"

#include "signal_browser_view.h"
#include "signal_browser_model_4.h"
#include "signal_graphics_item.h"

#include <QPaintEvent>
#include <QPainter>

#include <cmath>

#include <iostream>

namespace BioSig_
{

namespace PortingToQT4_
{

//-----------------------------------------------------------------------------
YAxisWidget::YAxisWidget(QWidget* parent, SignalBrowserModel& model, SignalBrowserView* browser)
  : QWidget (parent),
    signal_browser_model_(model),
    signal_browser_(browser)
{
}

//-----------------------------------------------------------------------------
QSize YAxisWidget::sizeHint () const
{
    return QSize (100, 0);
}

//-----------------------------------------------------------------------------
void YAxisWidget::addChannel(int32 channel_nr, SignalGraphicsItem* signal_item)
{
    if (signal_item)
    {
        channel_nr2signal_graphics_item_[channel_nr] = signal_item;
    }
}

//-----------------------------------------------------------------------------
void YAxisWidget::removeChannel(int32 channel_nr)
{
    QMap<int32, SignalGraphicsItem*>::iterator it = channel_nr2signal_graphics_item_.find(channel_nr);

    if (it != channel_nr2signal_graphics_item_.end())
    {
        channel_nr2signal_graphics_item_.erase(it);
    }
}

//-----------------------------------------------------------------------------
void YAxisWidget::paintEvent(QPaintEvent* event)
{
    int32 signal_height = signal_browser_model_.getSignalHeight();
    int32 signal_spacing = signal_browser_model_.getSignalSpacing();
    float64 intervall = signal_height + signal_spacing;

    int32 y_start = signal_browser_->getVisibleY();
    int32 y_end = y_start + height();
    int32 w = width();

    QPainter p(this);
    p.setPen(Qt::black);
    float64 float_y_start = floor(y_start / intervall) * intervall;
    float64 float_y_end = ceil(y_end / intervall) * intervall;
    QMap<int32, SignalGraphicsItem*>::iterator iter = channel_nr2signal_graphics_item_.begin();

    std::cout << "repaint.." << std::endl;
    for (float32 float_item_y = -y_start;
         float_item_y < float_y_end && iter != channel_nr2signal_graphics_item_.end();
         float_item_y += intervall, iter++)
    {
        p.setClipRect(0, (int32)float_item_y, w, intervall);

            float64 value_range = (iter.data()->getMaximum() - iter.data()->getMinimum()) /
                                  iter.data()->getYZoom();
            std::cout << "float_item_y = " << float_item_y << std::endl;
            float64 upper_value = iter.data()->getYOffset() + value_range / 2.0;
            p.drawLine(0, (int32)float_item_y + signal_height,
                       w - 1, (int32)float_item_y + signal_height);

            float64 y_grid_pixel_intervall = iter.data()->getYGridPixelIntervall();
            float64 y_grid_intervall = y_grid_pixel_intervall / signal_height * value_range;

            float64 value = (int32)((upper_value + y_grid_intervall) / y_grid_intervall) * y_grid_intervall;
            float64 y_float = (upper_value - value) * signal_height / value_range + float_item_y;
            for (;
                 value > upper_value - value_range - y_grid_intervall;
                 value -= y_grid_intervall, y_float += y_grid_pixel_intervall)
            {
                int32 y = (int32)(y_float + 0.5);
                p.drawLine(w - 5, y, w - 1, y);
                p.drawText(0, (int32)(y - 20) , w - 10, 40,
                       Qt::AlignRight | Qt::AlignVCenter, QString("%1")
                                                                .arg(qRound(value * 100) / 100.0));
            }
    }

}


}

}
