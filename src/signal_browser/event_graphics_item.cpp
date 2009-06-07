#include "event_graphics_item.h"

#include "signal_browser_model_4.h"
#include "signal_browser_view.h"
#include "../base/signal_buffer.h"

#include "../main_window_model.h"
#include "../event_color_manager.h"

#include <QRectF>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <iostream>

namespace BioSig_
{

namespace PortingToQT4_
{

//-----------------------------------------------------------------------------
EventGraphicsItem::EventGraphicsItem(uint32 id, SignalBuffer& buffer, SignalBrowserModel& model,
                    SignalBrowserView* browser)
: signal_browser_model_ (model),
  signal_buffer_ (buffer),
  id_ (id)
{
}

//-----------------------------------------------------------------------------
EventGraphicsItem::~EventGraphicsItem ()
{
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::setSize (int32 width, int32 height)
{
    width_ = width;
    height_ = height;
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::updateColor()
{
    EventColorManager& event_color_manager
        = signal_browser_model_.getMainWindowModel().getEventColorManager();
    color_ = event_color_manager.getEventColor(signal_buffer_.getEvent(id_)
                                                                ->getType());
}


//-----------------------------------------------------------------------------
QRectF EventGraphicsItem::boundingRect () const
{
    return QRectF (0, 0, width_, height_);
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    QRectF clip (option->exposedRect);

    // TODO: draw frame if event is selected
    //if (isSelected())
    //{
    //    p.drawRect(rect());
    //}
    painter->fillRect(clip, color_);
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
    //std::cout << "click on event" << std::endl;
    event->ignore();
}


}

}
