// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


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

namespace sigviewer
{

//-----------------------------------------------------------------------------
YAxisWidget::YAxisWidget (QWidget* parent, QSharedPointer<const SignalViewSettings> signal_view_settings)
  : QWidget (parent),
    channel_height_ (0),
    y_start_ (0),
    signal_view_settings_ (signal_view_settings)
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
void YAxisWidget::changeYStart(int32 y_start)
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
    bool channel_overlapping = signal_view_settings_->getChannelOverlapping();
    float64 signal_height = signal_view_settings_->getChannelHeight();
    if (channel_overlapping)
        signal_height =
                (signal_height +
                 (signal_height * (channel_nr2signal_graphics_item_.size() - 1)
                * (1.0 - signal_view_settings_->getChannelOverlapping())))
                / channel_nr2signal_graphics_item_.size();

    int32 y_end = y_start_ + height();


    if (signal_height < 1)
        return;

    QPainter painter (this);

//    float64 intervall = channel_height_;
    painter.translate(0, -y_start_);
    painter.drawLine(0, y_start_, 0, y_end);

    // labels
    float64 float_y_start = y_start_;//floor(static_cast<float64>(y_start) / intervall) * intervall;
    float64 float_y_end = y_end;//ceil(y_end / intervall) * intervall;
//    painter.translate (0, ((static_cast<float32>(channel_height_) / 2)) - y_start_);
    auto iter = channel_nr2signal_graphics_item_.begin();


//    int32 current_y_start = 0;
    for (float32 float_y = signal_height / 2;
         float_y < float_y_end && iter != channel_nr2signal_graphics_item_.end();
         float_y += signal_height, iter++)
    {
//        if (float_y > float_y_start)
//        {
//            int32 y = (int32)(float_y + 0.5);
//            painter.drawText(5, (int32)(y - signal_height /2) ,
//                             width() - 10, (int32)signal_height,
//                             Qt::AlignHCenter | Qt::AlignVCenter, tr("test"));
            paintYAxisLabels (&painter, iter.value()->getYOffset(),
                              iter.value()->getYGridPixelIntervall(),
                              iter.value()->getValueRangeFragment(),
                              iter.value()->getPhysicalDimensionString(),
                              signal_height);
            painter.translate (0, signal_height);
//        }
    }

    if (channel_overlapping)
        return;

//    QPointF upperLeftCorner (0, 0);
//    painter.translate(upperLeftCorner);
//    painter.translate(0, -y_start_);

    for (float32 float_y = 0;
         float_y <= signal_height * channel_nr2signal_graphics_item_.size();
         float_y += signal_height)
    {
        painter.drawLine(0, float_y, width() - 1, float_y);
    }


//    foreach (SignalGraphicsItem const* signal, channel_nr2signal_graphics_item_.values())
//    {
//        if (current_y_start >= y_start_ - intervall &&
//            current_y_start <= y_start_ + height ())
//            paintYAxisLabels (&painter, signal->getYOffset(),signal->getYGridPixelIntervall(),
//                              signal->getValueRangeFragment(),
//                              signal->getPhysicalDimensionString());
//        painter.translate (0, intervall);
//        current_y_start += intervall;
//    }
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
                                    QString const& unit_string, float64 height)
{
    int upper_border = height;
    int lower_border = -static_cast<int>(height / 2);

    painter->setClipping (true);
    painter->setClipRect (0, -1, width(), height + 2);

    painter->drawLine (0, 0, width() - 1, 0);
//    painter->drawLine (width () - 5, offset, width () - 1, offset);

//    paintYUnits (painter, unit_string);

    if (unit_string.size() > 4)
    {
        painter->rotate(-90);
        painter->drawText (5,  0, width(), height,
                           Qt::AlignLeft | Qt::AlignVCenter,
                           unit_string);
        painter->rotate(90);
    }
    else
        painter->drawText (5,  0, width() - 10, height,
                           Qt::AlignLeft | Qt::AlignVCenter,
                           unit_string);



//    painter->drawText  (0, offset, width () - 10, height,
//                        Qt::AlignRight | Qt::AlignVCenter,
//                        QString::number (0));

    if (y_grid_pixel_intervall < 1)
        return;

    while (y_grid_pixel_intervall < 11)
    {
        y_grid_pixel_intervall *= 2;
        value_range_fragment *= 2;
    }

    double value = 0;

    for (float64 value_y = offset + height / 2;
         value_y < height;
         value_y += y_grid_pixel_intervall)
    {
//        if (value_y > -static_cast<int>(height / 2))
//        {
            painter->drawLine (width () - 5, value_y, width () - 1, value_y);
            painter->drawText(0, value_y - 20, width () - 10, 40,
                             Qt::AlignRight | Qt::AlignVCenter,
                             QString::number (value));
//        }
        value -= value_range_fragment;
    }

    value = 0;
    for (float64 value_y = offset + height / 2 - y_grid_pixel_intervall;
         value_y > 0;
         value_y -= y_grid_pixel_intervall)
    {
        value += value_range_fragment;
//        if (value_y < height / 2)
//        {
            painter->drawLine (width () - 5, value_y, width () - 1, value_y);
            painter->drawText(0, value_y - 20, width () - 10, 40,
                             Qt::AlignRight | Qt::AlignVCenter,
                             QString::number (value));
//        }
    }
}

//-------------------------------------------------------------------
void YAxisWidget::paintYUnits (QPainter* painter, QString const& unit_string)
{
    if (unit_string.size() > 4)
    {
        painter->rotate(-90);
        painter->drawText (0,  5, width(), 60,
                           Qt::AlignLeft | Qt::AlignVCenter,
                           unit_string);
        painter->rotate(90);
    }
    else
        painter->drawText (5,  0, width() - 10, 40,
                           Qt::AlignLeft | Qt::AlignVCenter,
                           unit_string);
}

}
