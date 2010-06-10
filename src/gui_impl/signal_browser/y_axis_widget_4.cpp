#include "y_axis_widget_4.h"
#include "signal_graphics_item.h"
#include "../../gui/gui_action_factory.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QContextMenuEvent>

#include <cmath>

namespace BioSig_
{

//-----------------------------------------------------------------------------
YAxisWidget::YAxisWidget (QWidget* parent)
  : QWidget (parent),
    signal_height_ (0),
    y_start_ (0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
QSize YAxisWidget::sizeHint () const
{
    return QSize (70, 0);
}

//-----------------------------------------------------------------------------
void YAxisWidget::addChannel(ChannelID channel_nr, SignalGraphicsItem const* const signal_item)
{
    if (signal_item)
        channel_nr2signal_graphics_item_[channel_nr] = signal_item;
}

//-----------------------------------------------------------------------------
void YAxisWidget::removeChannel(ChannelID channel_nr)
{
    QMap<ChannelID, SignalGraphicsItem const*>::iterator it = channel_nr2signal_graphics_item_.find(channel_nr);

    if (it != channel_nr2signal_graphics_item_.end())
    {
        channel_nr2signal_graphics_item_.erase(it);
        update ();
    }
}

//-----------------------------------------------------------------------------
void YAxisWidget::changeSignalHeight (uint32 signal_height)
{
    signal_height_ = signal_height;
    update ();
}

//-----------------------------------------------------------------------------
void YAxisWidget::changeYStart (int y_start)
{
    y_start_ = y_start;
    update ();
}

//-----------------------------------------------------------------------------
void YAxisWidget::updateChannel (ChannelID)
{
    update ();
}

//-----------------------------------------------------------------------------
void YAxisWidget::paintEvent(QPaintEvent*)
{
    QPainter painter (this);

    float64 intervall = signal_height_;
    painter.translate (0, ((static_cast<float32>(signal_height_) / 2)) - y_start_);
    int32 current_y_start = 0;
    foreach (SignalGraphicsItem const* signal, channel_nr2signal_graphics_item_.values())
    {
        if (current_y_start >= y_start_ - intervall &&
            current_y_start <= y_start_ + height ())
            paintYAxisLabels (&painter, signal->getYOffset(), signal->getYZoom(), signal->getYGridPixelIntervall());
        painter.translate (0, intervall);
        current_y_start += intervall;
    }
}

//-----------------------------------------------------------------------------
void YAxisWidget::contextMenuEvent (QContextMenuEvent* event)
{
    QMenu menu;
    menu.addAction(GuiActionFactory::getInstance()->getQAction("Channels per Page..."));
    menu.exec (event->globalPos());
}


//-------------------------------------------------------------------
void YAxisWidget::paintYAxisLabels (QPainter* painter, float64 offset,
                                       float64 y_zoom, float64 y_grid_pixel_intervall)
{
    int upper_border = signal_height_ / 2;
    int lower_border = -static_cast<int>(signal_height_ / 2);

    painter->setClipping (true);
    painter->setClipRect (0, lower_border - 1, width(), upper_border - lower_border + 2);

    for (float64 value_y = offset;
         value_y < upper_border;
         value_y += y_grid_pixel_intervall)
    {
        if (value_y > -static_cast<int>(signal_height_ / 2))
        {
            painter->drawLine (width () - 5, value_y, width () - 1, value_y);
            painter->drawText(0, value_y - 20, width () - 10, 40,
                             Qt::AlignRight | Qt::AlignVCenter,
                             QString::number ((offset-value_y) / y_zoom));
        }
    }
    for (float64 value_y = offset - y_grid_pixel_intervall;
         value_y > lower_border;
         value_y -= y_grid_pixel_intervall)
    {
        if (value_y < signal_height_ / 2)
        {
            painter->drawLine (width () - 5, value_y, width () - 1, value_y);
            painter->drawText(0, value_y - 20, width () - 10, 40,
                             Qt::AlignRight | Qt::AlignVCenter,
                             QString::number ((offset-value_y) / y_zoom));
        }
    }
    painter->drawLine (0, upper_border,
                      width() - 1, upper_border);
}




}
