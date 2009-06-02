#include "event_graphics_item.h"

#include "signal_browser_model_4.h"
#include "signal_browser_view.h"
#include "../base/signal_buffer.h"

#include <QRectF>

namespace BioSig_
{

namespace PortingToQT4_
{

//-----------------------------------------------------------------------------
EventGraphicsItem::EventGraphicsItem(uint32 id, SignalBuffer& buffer, SignalBrowserModel& model,
                    SignalBrowserView* browser)
{
}

//-----------------------------------------------------------------------------
EventGraphicsItem::~EventGraphicsItem ()
{
}


//-----------------------------------------------------------------------------
QRectF EventGraphicsItem::boundingRect () const
{
    return QRectF (0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{

}


}

}
