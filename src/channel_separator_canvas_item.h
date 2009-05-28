#ifndef QT4_PORTED
// channel_separator_canvas_item.h

#ifndef CHANNEL_SEPARATOR_CANVAS_ITEM_H
#define CHANNEL_SEPARATOR_CANVAS_ITEM_H

#include "smart_canvas/smart_canvas_rectangle.h"
#include "signal_browser_model.h"

namespace BioSig_
{

class SmartCanvasView;
class SignalBrowser;

// channel separator canvas item
class ChannelSeparatorCanvasItem : public SmartCanvasRectangle
{
public:
    enum
    {
        RTTI = SmartCanvasRectangle::RTTI + 0x2
    };

    ChannelSeparatorCanvasItem(SignalBrowserModel& model,
                               SignalBrowser* browser);

    virtual int rtti() const;

protected:
    void smartDraw(QPainter& p, const QRect& clip);

private:
    SignalBrowserModel& signal_browser_model_;
    SignalBrowser* signal_browser_;
};

} // namespace BioSig_

#endif

#endif
