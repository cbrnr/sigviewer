#include "event_graphics_item.h"

#include "signal_browser_model_4.h"
#include "signal_browser_view.h"
#include "../base/signal_buffer.h"

#include "../main_window_model.h"
#include "../event_color_manager.h"
#include "../signal_browser_mouse_handling.h"

#include <QRectF>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <algorithm>
#include <iostream>

namespace BioSig_
{

namespace PortingToQT4_
{

// move mouse range
int EventGraphicsItem::move_mouse_range_ = 5;

//-----------------------------------------------------------------------------
EventGraphicsItem::EventGraphicsItem(uint32 id, SignalBuffer& buffer, SignalBrowserModel& model,
                    SignalBrowserView* browser)
: signal_browser_model_ (model),
  signal_buffer_ (buffer),
  id_ (id),
  state_ (STATE_NONE),
  is_selected_ (false)
{
}

//-----------------------------------------------------------------------------
EventGraphicsItem::~EventGraphicsItem ()
{
}

//-----------------------------------------------------------------------------
uint32 EventGraphicsItem                                                                                                                                                                                                                                                                                                                                    ::getId() const
{
    return id_;
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
    if (is_selected_)
    {
        painter->drawRect(boundingRect());
    }
    painter->fillRect(clip, color_);
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
    std::cout << "click on event " << id_ << std::endl;



    if (state_ != STATE_NONE)
    {
        return; // already in a edit state
    }

    switch(getMousePressAction(event))
    {
        case ACTION_NONE:
            event->ignore();
            break;
        /*case ACTION_MOVE_BEGIN:
            state_ = STATE_MOVE_BEGIN;
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            signal_browser_->getCanvasView()->viewport()
                                ->setCursor(QCursor(Qt::SizeHorCursor));
            break;
        case ACTION_MOVE_END:
            state_ = STATE_MOVE_END;
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            signal_browser_->getCanvasView()->viewport()
                                ->setCursor(QCursor(Qt::SizeHorCursor));
            break;
        case ACTION_SHIFT_TO_CHANNEL:
            state_ = STATE_SHIFT_TO_CHANNEL;
            last_shift_shown_nr_
                = (int32)((rect().y() + rect().height() / 2) /
                          (signal_browser_model_.getSignalHeight() +
                           signal_browser_model_.getSignalSpacing()));
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            signal_browser_->getCanvasView()->viewport()
                                        ->setCursor(QCursor(Qt::SizeVerCursor));
            break;
        case ACTION_COPY_SHIFT_TO_CHANNEL:
            {
                SignalEvent* event = signal_buffer_.getEvent(id_);
                SignalEvent new_event(*event);
                signal_browser_model_.addEvent(new_event);
                state_ = STATE_SHIFT_TO_CHANNEL;
                last_shift_shown_nr_
                    = (int32)((rect().y() + rect().height() / 2) /
                              (signal_browser_model_.getSignalHeight() +
                               signal_browser_model_.getSignalSpacing()));
                canvas_view->addEventListener(
                                        SmartCanvasView::MOUSE_RELEASE_EVENT |
                                        SmartCanvasView::MOUSE_MOVE_EVENT,
                                        this);
                signal_browser_->getCanvasView()->viewport()
                                        ->setCursor(QCursor(Qt::SizeVerCursor));
            }
            break;*/
        case ACTION_SELECT:
            {
                EventGraphicsItem* old_selected_item
                    = signal_browser_model_.getSelectedEventItem();
                if (old_selected_item)
                    old_selected_item->is_selected_ = false;
                is_selected_ = true;
                /*if (old_selected_item)
                {
                    canvas_view->removeEventListener(
                                        SmartCanvasView::MOUSE_PRESS_EVENT,
                                        old_selected_item);
                }
                canvas_view->addEventListener(
                                        SmartCanvasView::MOUSE_PRESS_EVENT,
                                        this);*/
                signal_browser_model_.setSelectedEventItem(this);
            }
            break;
    }
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mouseReleaseEvent (QGraphicsSceneMouseEvent * event)
{
}

//-----------------------------------------------------------------------------
// get mouse press action
EventGraphicsItem::Action EventGraphicsItem::getMousePressAction(QGraphicsSceneMouseEvent* e)
{
    SignalEvent* event = signal_buffer_.getEvent(id_);
    EventGraphicsItem* old_selected_item
        = signal_browser_model_.getSelectedEventItem();
    QPoint mouse_pos (e->pos().x(), e->pos().y());  //canvas_view->inverseWorldMatrix().map(e->pos());

    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    switch (SignalBrowserMouseHandling::getAction(e, mode))
    {
        case SignalBrowserMouseHandling::SELECT_RESIZE_EVENT_ACTION:
            {
                // select event
                if (!old_selected_item)
                {
                    return ACTION_SELECT;
                }

                QRectF old_rect = old_selected_item->boundingRect();
                int32 tmp = std::min(static_cast<int>(old_rect.width() / 2), move_mouse_range_);

                // move event end
                QRect move_end_rect(old_rect.right() - tmp, old_rect.top(),
                                    tmp + move_mouse_range_, old_rect.height());
                if (this == old_selected_item &&
                    move_end_rect.contains(mouse_pos))
                {
                    return ACTION_MOVE_END;
                }

                // move event begin
                QRect move_begin_rect(old_rect.left() - move_mouse_range_,
                                      old_rect.top(), tmp + move_mouse_range_,
                                      old_rect.height());
                if (this == old_selected_item &&
                    move_begin_rect.contains(mouse_pos))
                {
                    return ACTION_MOVE_BEGIN;
                }

                // select smallest clicked event
                if (old_selected_item != this &&
                    (!old_rect.contains(mouse_pos) ||
                     old_selected_item->boundingRect().width() > boundingRect().width() ||
                     (old_selected_item->boundingRect().width() == boundingRect().width() &&
                      old_selected_item->boundingRect().height() > boundingRect().height())))
                {
                    return ACTION_SELECT;
                }
            }
            break;
        case SignalBrowserMouseHandling::SHIFT_EVENT_TO_CHANNEL_ACTION:
            if (this == old_selected_item &&
                old_selected_item->boundingRect().contains(mouse_pos) &&
                event->getChannel() != SignalEvent::UNDEFINED_CHANNEL)
            {
                return ACTION_SHIFT_TO_CHANNEL;
            }
            break;
        case SignalBrowserMouseHandling::COPY_EVENT_TO_CHANNEL_ACTION:
            if (this == old_selected_item &&
                old_selected_item->boundingRect().contains(mouse_pos) &&
                event->getChannel() != SignalEvent::UNDEFINED_CHANNEL)
            {
                return ACTION_COPY_SHIFT_TO_CHANNEL;
            }
            break;
        default:
            break;
    }
    return ACTION_NONE;
}


}

}
