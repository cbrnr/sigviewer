#ifndef QT4_PORTED
// y_axis_widget.cpp

#include "y_axis_widget.h"
#include "signal_browser_model.h"
#include "signal_browser.h"
#include "smart_canvas/smart_canvas_view.h"
#include "signal_canvas_item.h"

#include <math.h>

#include <QPalette>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QSettings>

#define min(a, b) ((a) < (b) ? (a) : (b))

namespace BioSig_
{

// axis color
QColor YAxisWidget::axis_color_("#e0e0e0");

// constructor
YAxisWidget::YAxisWidget(SignalBrowserModel& model, SignalBrowser* browser)
: QWidget(browser),
  signal_browser_model_(model),
  signal_browser_(browser)
{
    QPalette palette;
    palette.setColor(backgroundRole(), axis_color_);
    setPalette(palette);

    QPixmap dummy(1,1);
    QPainter p(&dummy);
    QRect bounding = p.boundingRect(0, 0, 500, 500,
                                    Qt::AlignHCenter | Qt::AlignVCenter, "-888.88");
    setMinimumWidth(10 + bounding.width());
}

// get axis color
const QColor& YAxisWidget::getAxisColor()
{
    return axis_color_;
}

// load settings
void YAxisWidget::loadSettings()
{
    QSettings settings("SigViewer");
    axis_color_.setNamedColor(settings.value("YAxisWidget/axis_color",
                                             axis_color_.name()).toString());
}

// save settings
void YAxisWidget::saveSettings()
{
    QSettings settings("SigViewer");
    settings.setValue("YAxisWidget/axis_color", axis_color_.name());
}

// paint event
void YAxisWidget::paintEvent(QPaintEvent*)
{
    int32 signal_height = signal_browser_model_.getSignalHeight();
    int32 signal_spacing = signal_browser_model_.getSignalSpacing();
    float64 intervall = signal_height + signal_spacing;

    int32 y_start = signal_browser_->getCanvasView()->contentsY();
    int32 y_end = y_start + height();
    int32 w = width();

    QPainter p(this);
    p.translate(0, -y_start);
    p.drawLine(w - 1, y_start, w -1, y_end);

    //  all channels
//    bool even = true;
    p.setPen(Qt::black);
    float64 float_y_start = floor(y_start / intervall) * intervall;
    float64 float_y_end = ceil(y_end / intervall) * intervall;
    QMap<int32, SignalCanvasItem*>::iterator iter = channel_nr2signal_canvas_item_.begin();

    for (float32 float_item_y = 0;
         float_item_y < float_y_end && iter != channel_nr2signal_canvas_item_.end();
         float_item_y += intervall, iter++)
    {
//        even = !even;

        if (float_item_y > float_y_start - intervall)
        {
            // y range

            float64 value_range = (iter.data()->getMaximum() - iter.data()->getMinimum()) /
                                  iter.data()->getYZoom();

            float64 upper_value = iter.data()->getYOffset() + value_range / 2.0;

            p.setClipRect(0, (int32)float_item_y, w, intervall);
//            p.setPen(Qt::black);
            p.drawLine(0, (int32)float_item_y + signal_height,
                       w - 1, (int32)float_item_y + signal_height);
/*
            if (even)
            {
                p.setPen(Qt::red);
            }
            else
            {
                p.setPen(Qt::blue);
            }
*/

            // draw y markers and values
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

// add channel
void YAxisWidget::addChannel(int32 channel_nr, SignalCanvasItem* signal_item)
{
    if (signal_item)
    {
        channel_nr2signal_canvas_item_[channel_nr] = signal_item;
    }
}

// remove channel
void YAxisWidget::removeChannel(int32 channel_nr)
{
    QMap<int32, SignalCanvasItem*>::iterator it = channel_nr2signal_canvas_item_.find(channel_nr);

    if (it != channel_nr2signal_canvas_item_.end())
    {
        channel_nr2signal_canvas_item_.erase(it);
    }
}

} // namespace BioSig_

#endif
