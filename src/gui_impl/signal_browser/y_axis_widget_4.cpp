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

//!this section is set to be consistent with label_widget.cpp----------------
void YAxisWidget::paintEvent(QPaintEvent*)
{
    bool channel_overlapping = signal_view_settings_->getChannelOverlapping();
    float64 signal_height = signal_view_settings_->getChannelHeight();
    if (channel_overlapping)
        signal_height = (signal_height +
                 (signal_height * (channel_nr2signal_graphics_item_.size() - 1)
                * (1.0 - signal_view_settings_->getChannelOverlapping())))
                / channel_nr2signal_graphics_item_.size();

    int32 y_end = y_start_ + height();

    if (signal_height < 1)
        return;

    QPainter painter (this);
    painter.translate(0, -y_start_);
//    painter.setPen(QColor(0, 43, 130));
//    painter.drawLine(width() - 1, y_start_, width() - 1, y_end);
//    painter.setPen(Qt::black);

    float64 float_y_end = y_end;
    auto iter = channel_nr2signal_graphics_item_.begin();

    for (float32 float_y = signal_height / 2;
         float_y < float_y_end && iter != channel_nr2signal_graphics_item_.end();
         float_y += signal_height, iter++)
    {
        paintYAxisLabels (&painter, iter.value()->getYOffset(),
                          iter.value()->getYGridPixelIntervall(),
                          iter.value()->getValueRangeFragment(),
                          iter.value()->getPhysicalDimensionString(),
                          signal_height);
        painter.translate (0, signal_height);
    }

//    this is the bottom line
//    painter.setPen(QColor(0, 43, 130));
    painter.drawLine (0, 0, width() - 1, 0);
//    painter.setPen(Qt::black);

//    if (channel_overlapping)
//        return;

//    for (float32 float_y = 0;
//         float_y <= signal_height * channel_nr2signal_graphics_item_.size();
//         float_y += signal_height)
//    {
//        painter.drawLine(0, float_y, width() - 1, float_y);
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
                                    QString const& unit_string, float64 channelHeight)
{
    painter->setClipping (true);

    //Below, 0 is the relative upper boarder of each channel.
    //despite being an absolute value, because the painter coordinates are readjusted
    //automatically every time it moves to a new channel, hence 0 is the
    //relative position of the upper boarder of each channel
#define UPPER_BORDER 0

    painter->setClipRect (0, UPPER_BORDER - 1,          // -1 to include the black boarder as well
                          width(), channelHeight + 2);  //+2 include both upper and lower boarders

//    painter->setPen(QColor(0, 43, 130));
    painter->drawLine (0, UPPER_BORDER, width() - 1, UPPER_BORDER);
//    painter->setPen(Qt::black);

    paintYUnits (painter, unit_string, channelHeight);

    if (y_grid_pixel_intervall < 1)
        return;

    while (y_grid_pixel_intervall < 11)
    {
        y_grid_pixel_intervall *= 2;
        value_range_fragment *= 2;
    }

    double value = 0;

    //offset + channelHeight / 2 is the position where 0 is for each channel
    //However, 0 doesn't necessarily appear in the channel, if we are in
    //"zero line fitted" mode and if the mean is far from 0.

    //In the following for loop, we are still going to calculate from  the position of 0
    //even if 0 doesn't appear in the channel. Because in "view option" mode, the channel
    //can be scrolled up and down arbitrarily.
    for (float64 value_y = offset + channelHeight / 2; value_y < channelHeight;
         value_y += y_grid_pixel_intervall)
    {
        //But we only paint the calibration when they are within the channel height.
        //This can be dynamic if in the "view option" mode, since channels can be
        //scolled up and down.

        //Here, value starts with 0 means painter will draw 0 first, and subtract
        //value_range_fragment from value thereafter, so long as they are within channel
        //height from UPPER_BORDER
        if (value_y > UPPER_BORDER)
        {

            painter->drawLine (width () - 5, value_y, width () - 1, value_y);
            painter->drawText(0, value_y - 20, width () - 10, 40,
                              Qt::AlignRight | Qt::AlignVCenter,
                              QString::number (value));
        }
        value -= value_range_fragment;
    }

    //We finished painting 0 and all negative numbers, now reset value to 0
    value = 0;
    //Now paint the positive numbers
    for (float64 value_y = offset + channelHeight / 2 - y_grid_pixel_intervall;
         value_y > UPPER_BORDER; value_y -= y_grid_pixel_intervall)
    {
        value += value_range_fragment;
        //paint only if they position is within the range from UPPER_BORDER to
        //the bottom (UPPER_BORDER + channelHeight). This can be dynamic when
        //scrolling the channel.
        if (value_y < channelHeight)
        {
            painter->drawLine (width () - 5, value_y, width () - 1, value_y);
            painter->drawText(0, value_y - 20, width () - 10, 40,
                              Qt::AlignRight | Qt::AlignVCenter,
                              QString::number (value));
        }
    }
}

//-------------------------------------------------------------------
void YAxisWidget::paintYUnits (QPainter* painter, QString const& unit_string, float64 channelHeight)
{
    if (unit_string.size() > 4)
    {
        painter->rotate(-90);
        painter->drawText (5,  0, width(), channelHeight,
                           Qt::AlignLeft | Qt::AlignVCenter,
                           unit_string);
        painter->rotate(90);
    }
    else
        painter->drawText (5,  0, width() - 10, channelHeight,
                           Qt::AlignLeft | Qt::AlignVCenter,
                           unit_string);
}

}
