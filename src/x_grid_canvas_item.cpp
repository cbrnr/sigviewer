// x_grid_canvas_item.cpp

#include "x_grid_canvas_item.h"
#include "smart_canvas/smart_canvas_view.h"
#include "signal_browser.h"

#include <math.h>

#include <QPainter>

namespace BioSig_
{

// constructor
XGridCanvasItem::XGridCanvasItem(SignalBrowserModel& model,
                                 SignalBrowser* browser)
: SmartCanvasRectangle(browser->getCanvas()),
  signal_browser_model_(model),
  signal_browser_(browser)
{
    // nothing
}

// rtti
int XGridCanvasItem::rtti() const
{
    return RTTI;
}

// smart draw
void XGridCanvasItem::smartDraw(QPainter& p, const QRect& clip)
{
    p.setPen(Qt::lightGray);
    int32 y_start = clip.y();
    int32 y_end = y_start + clip.height();
    int32 x_start = clip.x() - 1;               // 1 pixel left
    int32 x_end = x_start + clip.width() + 1;   // 1 pixel right
    float64 intervall = signal_browser_model_.getXGridPixelIntervall();
    float32 float_x = ceil(x_start / intervall) * intervall;
    int32 x;

    while ((x = (int32)(float_x + 0.5)) < x_end)
    {
        if (x > x_start)
        {
            p.drawLine(x, y_start, x, y_end);
        }
        float_x += intervall;
    }
}

} // namespace BioSig_

