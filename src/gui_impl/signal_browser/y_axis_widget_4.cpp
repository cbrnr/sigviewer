#include "y_axis_widget_4.h"
#include "signal_graphics_item.h"
#include "gui/gui_action_factory.h"
#include "base/math_utils.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QContextMenuEvent>

#include <cmath>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
YAxisWidget::YAxisWidget (QWidget* parent)
  : QWidget (parent),
    channel_height_ (0),
    y_start_ (0)
{
    // nothing to do here
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
void YAxisWidget::changeSignalHeight (unsigned signal_height)
{
    channel_height_ = signal_height;
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

    float64 intervall = channel_height_;
    painter.translate (0, ((static_cast<float32>(channel_height_) / 2)) - y_start_);
    int32 current_y_start = 0;
    foreach (SignalGraphicsItem const* signal, channel_nr2signal_graphics_item_.values())
    {
        if (current_y_start >= y_start_ - intervall &&
            current_y_start <= y_start_ + height ())
            paintYAxisLabels (&painter, signal->getYOffset(),signal->getYGridPixelIntervall(),
                              signal->getValueRangeFragment(),
                              signal->getPhysicalDimensionString());
        painter.translate (0, intervall);
        current_y_start += intervall;
    }
}

//-----------------------------------------------------------------------------
void YAxisWidget::contextMenuEvent (QContextMenuEvent* event)
{
    QMenu menu;
    menu.addAction(GuiActionFactory::getInstance()->getQAction("Channels per Page..."));
    menu.addSeparator();
    QAction* visibility_action = menu.addAction("Y Axis");
    visibility_action->setCheckable (true);
    visibility_action->setChecked (true);
    connect (visibility_action, SIGNAL(triggered(bool)), SLOT(setVisible(bool)));
    menu.exec (event->globalPos());
}


//-------------------------------------------------------------------
void YAxisWidget::paintYAxisLabels (QPainter* painter, float64 offset,
                                    float64 y_grid_pixel_intervall,
                                    double value_range_fragment,
                                    QString const& unit_string)
{
    int upper_border = channel_height_ / 2;
    int lower_border = -static_cast<int>(channel_height_ / 2);

    painter->setClipping (true);
    painter->setClipRect (0, lower_border - 1, width(), upper_border - lower_border + 2);

    painter->drawLine (0, upper_border,
                       width() - 1, upper_border);
    painter->drawLine (width () - 5, offset, width () - 1, offset);

    paintYUnits (painter, unit_string);

    painter->drawText (0, offset - 20, width () - 10, 40,
                       Qt::AlignRight | Qt::AlignVCenter,
                       QString::number (0));
    if (y_grid_pixel_intervall < 1)
        return;

    while (y_grid_pixel_intervall < 11)
    {
        y_grid_pixel_intervall *= 2;
        value_range_fragment *= 2;
    }

    double value = 0;

    for (float64 value_y = offset;
         value_y < upper_border;
         value_y += y_grid_pixel_intervall)
    {
        if (value_y > -static_cast<int>(channel_height_ / 2))
        {
            painter->drawLine (width () - 5, value_y, width () - 1, value_y);
            painter->drawText(0, value_y - 20, width () - 10, 40,
                             Qt::AlignRight | Qt::AlignVCenter,
                             QString::number (value));
        }
        value -= value_range_fragment;
    }

    value = 0;
    for (float64 value_y = offset - y_grid_pixel_intervall;
         value_y > lower_border;
         value_y -= y_grid_pixel_intervall)
    {
        value += value_range_fragment;
        if (value_y < channel_height_ / 2)
        {
            painter->drawLine (width () - 5, value_y, width () - 1, value_y);
            painter->drawText(0, value_y - 20, width () - 10, 40,
                             Qt::AlignRight | Qt::AlignVCenter,
                             QString::number (value));
        }
    }
}

//-------------------------------------------------------------------
void YAxisWidget::paintYUnits (QPainter* painter, QString const& unit_string)
{
    if (unit_string.size() > 4)
    {
        painter->rotate(-90);
        painter->drawText (0,  5, width(), 60,
                           Qt::AlignLeft | Qt::AlignTop,
                           unit_string);
        painter->rotate(90);
    }
    else
        painter->drawText (5,  0, width() - 10, 40,
                           Qt::AlignLeft | Qt::AlignTop,
                           unit_string);
}




}
