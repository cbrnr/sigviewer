#ifndef QT4_PORTED
// channel_separator_canvas_item.cpp

#include "channel_separator_canvas_item.h"
#include "smart_canvas/smart_canvas_view.h"
#include "signal_browser.h"

#include <math.h>
#include <QPainter>

namespace BioSig_
{

// constructor
ChannelSeparatorCanvasItem::ChannelSeparatorCanvasItem(
                            SignalBrowserModel& model, SignalBrowser* browser)
: SmartCanvasRectangle(browser->getCanvas()),
  signal_browser_model_(model),
  signal_browser_(browser)
{
    // nothing
}

// rtti
int ChannelSeparatorCanvasItem::rtti() const
{
    return RTTI;
}

// smart draw
void ChannelSeparatorCanvasItem::smartDraw(QPainter& p, const QRect& clip)
{
    p.setPen(Qt::black);

    int32 signal_height = signal_browser_model_.getSignalHeight();
    int32 signal_spacing = signal_browser_model_.getSignalSpacing();
    float64 intervall = signal_height + signal_spacing;
    int32 x_start = clip.x();
    int32 x_end = x_start + clip.width();
    int32 y_start = clip.y() - 1;               // 1 pixel above
    int32 y_end = y_start + clip.height() + 1;  // 1 pixel below
    float64 float_y = ceil((y_start - signal_height - signal_spacing / 2)
                           / intervall) *
                      intervall + signal_height + signal_spacing / 2;

    int32 y;
    while((y = (int32)((float_y + 0.5))) < y_end)
    {
        if (y > y_start)
        {
            p.drawLine(x_start, y, x_end, y);
        }

        float_y += intervall;
    }
}

} // namespace BioSig_

#endif
