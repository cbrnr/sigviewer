// x_grid_canvas_item.h

#ifndef X_GRID_CANVAS_ITEM_H
#define X_GRID_CANVAS_ITEM_H

#include "smart_canvas/smart_canvas_rectangle.h"
#include "signal_browser_model.h"

namespace BioSig_
{

class SmartCanvasView;
class SignalBrowser;

// x grid canvas item
class XGridCanvasItem : public SmartCanvasRectangle
{

public:

    enum
    {
        RTTI = SmartCanvasRectangle::RTTI + 0x6
    };

    XGridCanvasItem(SignalBrowserModel& model, SignalBrowser* browser);

    virtual int rtti() const;

protected:

    void smartDraw(QPainter& p, const QRect& clip);

private:

    SignalBrowserModel& signal_browser_model_;
    SignalBrowser* signal_browser_;
};

} // namespace BioSig_

#endif

