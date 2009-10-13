#include "event_graphics_item.h"

#include "signal_browser_model_4.h"
#include "signal_browser_view.h"
#include "resize_event_undo_command.h"
#include "event_context_menu.h"
#include "../base/signal_buffer.h"
#include "../base/signal_event.h"

#include "../command_stack.h"
#include "../main_window_model.h"
#include "../event_color_manager.h"
#include "../signal_browser_mouse_handling.h"
#include "../main_window_model.h"
#include "../main_window.h"
#include "../base/event_table_file_reader.h"

#include <QRectF>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMutex>

#include <algorithm>
#include <iostream>

namespace BioSig_
{

namespace PortingToQT4_
{

// move mouse range
int EventGraphicsItem::move_mouse_range_ = 5;
QMutex EventGraphicsItem::event_handling_mutex_;
QMutex EventGraphicsItem::context_menu_mutex_;
QSharedPointer<EventContextMenu> EventGraphicsItem::context_menu_;

//-----------------------------------------------------------------------------
EventGraphicsItem::EventGraphicsItem(SignalBuffer& buffer, SignalBrowserModel& model,
                    SignalBrowserView* browser, QSharedPointer<SignalEvent> signal_event)
: signal_browser_model_ (model),
  signal_buffer_ (buffer),
  state_ (STATE_NONE),
  is_selected_ (false),
  signal_event_ (signal_event)
{
    setAcceptHoverEvents (true);
}

//-----------------------------------------------------------------------------
EventGraphicsItem::~EventGraphicsItem ()
{
}

//-----------------------------------------------------------------------------
uint32 EventGraphicsItem                                                                                                                                                                                                                                                                                                                                    ::getId() const
{
    return signal_event_->getId();
}


//-----------------------------------------------------------------------------
void EventGraphicsItem::setSize (int32 width, int32 height)
{
    width_ = width;
    height_ = height;
}


//-----------------------------------------------------------------------------
void EventGraphicsItem::startMouseMoveEnd ()
{
    state_ = STATE_MOVE_END;
    setCursor(QCursor(Qt::SizeHorCursor));
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::setSelected (bool selected)
{
    state_ = STATE_NONE;
    QSharedPointer<EventGraphicsItem> old_selected_item
        = signal_browser_model_.getSelectedEventItem();
    if (!(old_selected_item.isNull()))
    {
        old_selected_item->is_selected_ = false;
        old_selected_item->state_ = STATE_NONE;
        //old_selected_item->update();
    }
    is_selected_ = true;
    signal_browser_model_.setSelectedEventItem(signal_browser_model_.getEventItem(signal_event_->getId()));
    //update();
    scene()->update(0, 0, scene()->width(), scene()->height());
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent> EventGraphicsItem::getSignalEvent ()
{
    return signal_event_;
}


//-----------------------------------------------------------------------------
void EventGraphicsItem::updateColor()
{
    EventColorManager& event_color_manager
        = signal_browser_model_.getMainWindowModel().getEventColorManager();
    color_ = event_color_manager.getEventColor(signal_event_->getType());
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::displayContextMenu (QGraphicsSceneContextMenuEvent * event)
{
    context_menu_mutex_.lock();
    context_menu_->finaliseAndShowMenu (event);
    context_menu_mutex_.unlock();
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
    //painter->fillRect(clip, color_);
    painter->fillRect(boundingRect(), color_);
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
//    std::cout << "clicked on event " << id_ << std::endl;
//    event->ignore();
    event_handling_mutex_.lock();

    if (state_ != STATE_NONE)
    {
        event_handling_mutex_.unlock();
        return; // already in a edit state
    }

    switch(getMousePressAction(event))
    {
        case ACTION_NONE:
            event->ignore();
            state_ = STATE_NONE;
            break;
        case ACTION_MOVE_BEGIN:
            state_ = STATE_MOVE_BEGIN;
            std::cout << "move begin" << std::endl;
            /*
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);*/
            setCursor(QCursor(Qt::SizeHorCursor));
            break;
        case ACTION_MOVE_END:
            state_ = STATE_MOVE_END;
            //std::cout << "move end " << id_ << std::endl;
            /*canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);*/
            setCursor(QCursor(Qt::SizeHorCursor));
            break;
        /*case ACTION_SHIFT_TO_CHANNEL:
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
                setSelected(true);
                /*state_ = STATE_NONE;
                QSharedPointer<EventGraphicsItem> old_selected_item
                    = signal_browser_model_.getSelectedEventItem();
                if (!(old_selected_item.isNull()))
                {
                    old_selected_item->is_selected_ = false;
                    old_selected_item->update();
                }
                is_selected_ = true;
                signal_browser_model_.setSelectedEventItem(signal_browser_model_.getEventItem(signal_event_->getId()));
                update();*/
            }
            break;
    }
    event_handling_mutex_.unlock();
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mouseMoveEvent (QGraphicsSceneMouseEvent * mouse_event)
{
    QPoint mouse_pos (mouse_event->scenePos().x(), mouse_event->scenePos().y()); // event->canvas_view->inverseWorldMatrix().map(e->pos());
    switch(state_)
    {
        case STATE_NONE:
            break;
        case STATE_MOVE_BEGIN:
            {
//                setPos(mouse_event->scenePos().x(), pos().y());
                int32 diff = (mouse_event->pos().x() - mouse_event->lastPos().x());
                setPos (pos().x() + diff, pos().y());
//                if (!(scene()->views().first()->visibleRegion().contains(mouse_event->scenePos().toPoint())))
//                    std::cout << "not visible" << std::endl;
                width_ -= diff;
                scene()->update(mouse_pos.x(), pos().y(), width_ + diff, height_);
                update();
            }
            break;
        case STATE_MOVE_END:
            {
//                if (!(scene()->views().first()->visibleRegion().contains(mouse_event->scenePos().toPoint())))
//                    std::cout << "not visible" << std::endl;
                int32 diff = (mouse_event->pos().x() - mouse_event->lastPos().x());
                width_ += diff;
                scene()->update(mouse_pos.x(), pos().y(), width_ - diff, height_);
                update();
            }
            break;
        /*case STATE_SHIFT_TO_CHANNEL:
            int32 shown_nr;
            shown_nr = (int32)(mouse_pos.y() /
                               (signal_browser_model_.getSignalHeight() +
                                signal_browser_model_.getSignalSpacing()));
            if (shown_nr != last_shift_shown_nr_)
            {
                event->setChannel(signal_browser_model_.getChannelNr(shown_nr));
                signal_browser_model_.setEventChanged(id_);
                last_shift_shown_nr_ = shown_nr;
            }
            break;*/
    }
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mouseReleaseEvent (QGraphicsSceneMouseEvent * event)
{
    QPoint mouse_pos (event->scenePos().x(), event->scenePos().y());
    switch(state_)
    {
        case STATE_MOVE_BEGIN:
        {
            uint32 pos = x() * signal_buffer_.getEventSamplerate() / signal_browser_model_.getPixelPerSec();
            int32 dur = width_ * (signal_buffer_.getEventSamplerate() / signal_browser_model_.getPixelPerSec());

            ResizeEventUndoCommand* command = new ResizeEventUndoCommand (signal_browser_model_, signal_event_, pos, dur);
            CommandStack::instance().executeEditCommand (command);
        }
        break;
        case STATE_MOVE_END:
        {
            int32 dur = width_ * (signal_buffer_.getEventSamplerate() / signal_browser_model_.getPixelPerSec());
            ResizeEventUndoCommand* command = new ResizeEventUndoCommand (signal_browser_model_, signal_event_, signal_event_->getPosition(), dur);
            CommandStack::instance().executeEditCommand (command);
        }
        break;
    }
    setCursor(QCursor(Qt::ArrowCursor));
    state_ = STATE_NONE;
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::hoverMoveEvent (QGraphicsSceneHoverEvent * event )
{
    if (is_selected_)
    {
        if ((event->scenePos().x() > pos().x() && event->scenePos().x() < pos().x() + 5)
            || (event->scenePos().x() < pos().x() + width_ && event->scenePos().x() > pos().x() + width_ - 5))
            setCursor(QCursor(Qt::SizeHorCursor));
        else
            setCursor(QCursor(Qt::ArrowCursor));
    }
}


//-----------------------------------------------------------------------------
void EventGraphicsItem::contextMenuEvent (QGraphicsSceneContextMenuEvent * event)
{
    context_menu_mutex_.lock();
    event->ignore();
    if (context_menu_.isNull())
        context_menu_ = QSharedPointer<EventContextMenu> (new EventContextMenu (signal_browser_model_));
    context_menu_->addEvent(signal_browser_model_.getEventItem(signal_event_->getId()));
    context_menu_mutex_.unlock();
//    context_menu_->addAction();
//    context_menu_.exec(event->screenPos());
/*    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    if (mode != SignalBrowserModel::MODE_POINTER &&
        mode != SignalBrowserModel::MODE_NEW)
    {
        return;
    }
    //std::cout << "context menu for event " << id_ << std::endl;
    QMenu* menu = new QMenu();

//    MainWindow* main_window = signal_browser_model_.getMainWindowModel().getMainWindow();
//    main_window->addActionTo(menu, "
//    QAction *removeAction = menu.addAction("Remove");
//    QAction *markAction = menu.addAction("Mark");
//    QAction *selectedAction = menu.exec(event->screenPos());


//    if (menu.count() > 0)
//    {
//        menu.insertSeparator();
//    }


    QSharedPointer<EventGraphicsItem> old_selected_item;
    old_selected_item = signal_browser_model_.getSelectedEventItem();
    if (old_selected_item == this)
    {
        MainWindow* main_window
            = signal_browser_model_.getMainWindowModel().getMainWindow();
        main_window->addActionTo(menu, "edit_to_all_channels_action_");
        main_window->addActionTo(menu, "edit_copy_to_channels_action_");
        main_window->addActionTo(menu, "edit_delete_action_");
        main_window->addActionTo(menu, "edit_change_channel_action_");
        main_window->addActionTo(menu, "edit_change_type_action_");
    }
    else
    {
        EventTableFileReader& event_table_reader
         = signal_browser_model_.getMainWindowModel().getEventTableFileReader();
        QString event_name = event_table_reader.getEventName(signal_event_->getType());
        if (event_name.isEmpty())
        {
            event_name = "Undefined";
        }
        menu->addAction(QString("&Select ") + event_name);
    }
    menu->exec(event->screenPos());*/

}


//-----------------------------------------------------------------------------
// get mouse press action
EventGraphicsItem::Action EventGraphicsItem::getMousePressAction(QGraphicsSceneMouseEvent* e)
{
    QSharedPointer<EventGraphicsItem> old_selected_item
        = signal_browser_model_.getSelectedEventItem();
    QPoint mouse_pos (e->scenePos().x(), e->scenePos().y());  //canvas_view->inverseWorldMatrix().map(e->pos());


    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    switch (SignalBrowserMouseHandling::getAction(e, mode))
    {
        case SignalBrowserMouseHandling::SELECT_RESIZE_EVENT_ACTION:
            {
                // select event
                if (old_selected_item.isNull())
                {
                    std::cout << "select" << std::endl;
                    return ACTION_SELECT;
                }

                QRectF old_rect = old_selected_item->sceneBoundingRect();
                int32 tmp = std::min(static_cast<int>(old_rect.width() / 2), move_mouse_range_);

                // move event end
                QRect move_end_rect(old_rect.right() - tmp, old_rect.top(),
                                    tmp + move_mouse_range_, old_rect.height());
                if (signal_event_->getId() == old_selected_item->signal_event_->getId() &&
                    move_end_rect.contains(mouse_pos))
                {
                    std::cout << "action move end" << std::endl;
                    return ACTION_MOVE_END;
                }

                // move event begin
                QRect move_begin_rect(old_rect.left() - move_mouse_range_,
                                      old_rect.top(), tmp + move_mouse_range_,
                                      old_rect.height());
                if (signal_event_->getId() == old_selected_item->signal_event_->getId() &&
                    move_begin_rect.contains(mouse_pos))
                {
                    std::cout << "action move begin" << std::endl;
                    return ACTION_MOVE_BEGIN;
                }

                // select smallest clicked event
                if (old_selected_item->signal_event_->getId() != signal_event_->getId() &&
                    (!old_rect.contains(mouse_pos) ||
                     old_selected_item->sceneBoundingRect().width() > sceneBoundingRect().width() ||
                     (old_selected_item->sceneBoundingRect().width() == sceneBoundingRect().width() &&
                      old_selected_item->sceneBoundingRect().height() > sceneBoundingRect().height())))
                {
                    return ACTION_SELECT;
                }

            }
            break;
        case SignalBrowserMouseHandling::SHIFT_EVENT_TO_CHANNEL_ACTION:
            if (this == old_selected_item.data() &&
                old_selected_item->boundingRect().contains(mouse_pos) &&
                signal_event_->getChannel() != SignalEvent::UNDEFINED_CHANNEL)
            {
                return ACTION_SHIFT_TO_CHANNEL;
            }
            break;
        case SignalBrowserMouseHandling::COPY_EVENT_TO_CHANNEL_ACTION:
            if (this == old_selected_item.data() &&
                old_selected_item->boundingRect().contains(mouse_pos) &&
                signal_event_->getChannel() != SignalEvent::UNDEFINED_CHANNEL)
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
