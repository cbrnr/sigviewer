#include "event_graphics_item.h"

#include "signal_browser_model_4.h"
#include "editing_commands/resize_event_undo_command.h"
#include "event_context_menu.h"
#include "base/signal_event.h"

#include "gui_impl/signal_browser_mouse_handling.h"

#include <QRectF>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMutex>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>

#include <algorithm>
#include <cmath>

namespace SigViewer_
{

// move mouse range
int EventGraphicsItem::move_mouse_range_ = 5;
QMutex EventGraphicsItem::event_handling_mutex_;
QMutex EventGraphicsItem::context_menu_mutex_;
EventContextMenu* EventGraphicsItem::context_menu_ = 0;

//-----------------------------------------------------------------------------
EventGraphicsItem::EventGraphicsItem (SignalBrowserModel& model,
                                      QSharedPointer<SignalViewSettings const> signal_view_settings,
                                      QSharedPointer<SignalEvent const> signal_event,
                                      QSharedPointer<EventManager> event_manager,
                                      QSharedPointer<CommandExecuter> command_executer,
                                      QSharedPointer<ColorManager const> color_manager)
: signal_browser_model_ (model),
  signal_view_settings_ (signal_view_settings),
  event_manager_ (event_manager),
  command_executer_ (command_executer),
  state_ (STATE_NONE),
  is_selected_ (false),
  signal_event_ (signal_event),
  color_manager_ (color_manager)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
EventGraphicsItem::~EventGraphicsItem ()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
int32 EventGraphicsItem                                                                                                                                                                                                                                                                                                                                    ::getId() const
{
    return signal_event_->getId();
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::setSelected (bool selected)
{
    state_ = STATE_NONE;
    is_selected_ = selected;
    if (scene ())
        scene()->update(pos().x() - 5, pos().y() - 5, width_ + 10, height_ + 10);
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> EventGraphicsItem::getSignalEvent () const
{
    return signal_event_;
}

//-----------------------------------------------------------------------------
bool EventGraphicsItem::displayContextMenu (QGraphicsSceneContextMenuEvent* event,
                                            QMenu* channel_menu)
{
    context_menu_mutex_.lock();
    bool menu_shown = false;
    if (!context_menu_)
        menu_shown = false;
    else if (context_menu_->getNumberOfEvents())
    {
        context_menu_->finaliseAndShowContextMenu (event, channel_menu);
        menu_shown = true;
    }
    context_menu_mutex_.unlock();
    return menu_shown;
}

//-----------------------------------------------------------------------------
bool EventGraphicsItem::displaySelectionMenu (QGraphicsSceneMouseEvent* event)
{
    context_menu_mutex_.lock();
    bool menu_shown = false;
    if (!context_menu_)
    {
        menu_shown = false;
    }
    else if (context_menu_->getNumberOfEvents())
    {
        context_menu_->finaliseAndShowSelectionMenu(event);
        menu_shown = true;
    }
    context_menu_mutex_.unlock();
    return menu_shown;
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::updateToSignalEvent ()
{
    float64 pixel_per_sample = signal_view_settings_->getPixelsPerSample();
    QRectF old_rect;
    if (scene ())
        old_rect = this->sceneBoundingRect();
    width_ = pixel_per_sample * signal_event_->getDuration() + 0.5;
    if (width_ < 2)
        width_ = 2;
    int32 x_pos = pixel_per_sample * signal_event_->getPosition();
    int32 y_pos = 0;

    if (signal_event_->getChannel() == UNDEFINED_CHANNEL)
        height_ = (signal_view_settings_->getChannelHeight()) * signal_browser_model_.getShownChannels().size();
    else
    {
        height_ = signal_view_settings_->getChannelHeight();
        y_pos = signal_browser_model_.getYPosOfChannel(signal_event_->getChannel());
    }


    color_ = color_manager_->getEventColor(signal_event_->getType());

    /// FIXXME: REMOVE MAGIC NUMBER
    setZValue (5 + signal_event_->getType() / 100000.0);

    setPos (x_pos, y_pos);

    if (scene())
        scene()->update(old_rect);
    update ();
}


//-----------------------------------------------------------------------------
QRectF EventGraphicsItem::boundingRect () const
{
    return QRectF (0, 0, width_, height_);
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::paint (QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget*)
{
    QRect clip (option->exposedRect.toRect());

    if (is_selected_)
    {
        painter->drawRect(boundingRect());
    }

    painter->fillRect(clip, color_);
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
    event_handling_mutex_.lock();

    if (state_ != STATE_NONE)
    {
        event_handling_mutex_.unlock();
        return;
    }

    switch(getMousePressAction(event))
    {
        case ACTION_NONE:
            event->ignore();
            state_ = STATE_NONE;
            break;
        case ACTION_MOVE_BEGIN:
            state_ = STATE_MOVE_BEGIN;
            setCursor(QCursor(Qt::SizeHorCursor));
            emit mouseMoving (true);
            break;
        case ACTION_MOVE_END:
            state_ = STATE_MOVE_END;
            setCursor(QCursor(Qt::SizeHorCursor));
            emit mouseMoving (true);
            break;
        case ACTION_SELECT:
            {
                addContextMenuEntry ();
                event->ignore();
            }
            break;
        default:
            break;
    }
    event_handling_mutex_.unlock();
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mouseMoveEvent (QGraphicsSceneMouseEvent* mouse_event)
{
    float32 pixel_per_sample = signal_view_settings_->getPixelsPerSample();
    int32 mouse_pos_rounded = 0.5 + (mouse_event->scenePos().x() / pixel_per_sample);
    mouse_pos_rounded *= pixel_per_sample;

    switch(state_)
    {
        case STATE_NONE:
            break;
        case STATE_MOVE_BEGIN:
            {
                int32 old_pos = pos().x();
                int32 new_pos = mouse_pos_rounded;
                width_ = width_ - (new_pos - old_pos);
                if (width_ < 0)
                    width_ = 0;
                setPos (new_pos, pos().y());
                emit mouseAtSecond (static_cast<float>(pos().x())  / (pixel_per_sample * event_manager_->getSampleRate()));
            }
            break;
        case STATE_MOVE_END:
            {
                int32 old_pos_end = pos().x() + width_;
                int32 diff = mouse_pos_rounded - old_pos_end;
                width_ = width_ + diff;
                if (width_ < 0)
                    width_ = 0;

                if (diff > 0)
                    scene()->update (pos().x() + width_ - diff - 10, pos().y(),
                                     diff + 20, height_);
                else
                    scene()->update (pos().x() + width_ - 10, pos().y(),
                                     20 - diff, height_);
                emit mouseAtSecond (static_cast<float>(pos().x() + width_)  / (pixel_per_sample * event_manager_->getSampleRate()));
            }
            break;
    default:
            break;
    }
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::mouseReleaseEvent (QGraphicsSceneMouseEvent*)
{
    float32 pixel_per_sample = signal_view_settings_->getPixelsPerSample();
    switch(state_)
    {
        case STATE_MOVE_BEGIN:
        {
            int32 sample_pos = (static_cast<float32>(pos().x()) / pixel_per_sample) + 0.5;
            int32 dur = (static_cast<float32>(width_) / pixel_per_sample) + 0.5;
            ResizeEventUndoCommand* command = new ResizeEventUndoCommand (event_manager_, signal_event_->getId(), sample_pos, dur);
            command_executer_->executeCommand (command);
        }
        break;
        case STATE_MOVE_END:
        {
            int32 dur = (static_cast<float32>(width_) / pixel_per_sample) + 0.5;
            ResizeEventUndoCommand* command = new ResizeEventUndoCommand (event_manager_, signal_event_->getId(), signal_event_->getPosition(), dur);
            command_executer_->executeCommand (command);
        }
        break;
    default:
        break;
    }
    setCursor(QCursor(Qt::ArrowCursor));
    state_ = STATE_NONE;
    emit mouseMoving (false);
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::contextMenuEvent (QGraphicsSceneContextMenuEvent * event)
{
    if (signal_browser_model_.getMode() != MODE_VIEW_OPTIONS)
        addContextMenuEntry ();
    event->ignore();
}


//-----------------------------------------------------------------------------
EventGraphicsItem::Action EventGraphicsItem::getMousePressAction(QGraphicsSceneMouseEvent* e)
{
    EventGraphicsItem* old_selected_item
        = signal_browser_model_.getSelectedEventItem();
    QPoint mouse_pos (e->scenePos().x(), e->scenePos().y());


    SignalVisualisationMode mode = signal_browser_model_.getMode();
    switch (SignalBrowserMouseHandling::getAction(e, mode))
    {
        case SignalBrowserMouseHandling::SELECT_RESIZE_EVENT_ACTION:
            {
                // select event
                if (!old_selected_item)
                {
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
                    return ACTION_MOVE_END;
                }

                // move event begin
                QRect move_begin_rect(old_rect.left() - move_mouse_range_,
                                      old_rect.top(), tmp + move_mouse_range_,
                                      old_rect.height());
                if (signal_event_->getId() == old_selected_item->signal_event_->getId() &&
                    move_begin_rect.contains(mouse_pos))
                {
                    return ACTION_MOVE_BEGIN;
                }

                return ACTION_SELECT;
            }
            break;
        case SignalBrowserMouseHandling::SHIFT_EVENT_TO_CHANNEL_ACTION:
            if (this == old_selected_item &&
                old_selected_item->boundingRect().contains(mouse_pos) &&
                signal_event_->getChannel() != UNDEFINED_CHANNEL)
            {
                return ACTION_SHIFT_TO_CHANNEL;
            }
            break;
        case SignalBrowserMouseHandling::COPY_EVENT_TO_CHANNEL_ACTION:
            if (this == old_selected_item &&
                old_selected_item->boundingRect().contains(mouse_pos) &&
                signal_event_->getChannel() != UNDEFINED_CHANNEL)
            {
                return ACTION_COPY_SHIFT_TO_CHANNEL;
            }
            break;
        default:
            break;
    }
    return ACTION_NONE;
}

//-----------------------------------------------------------------------------
void EventGraphicsItem::addContextMenuEntry ()
{
    context_menu_mutex_.lock();
    if (!context_menu_)
        context_menu_ = new EventContextMenu (signal_browser_model_, event_manager_);
    else
    {
        if (context_menu_->getNumberOfEvents() == 0)
        {
            delete context_menu_;
            context_menu_ = new EventContextMenu (signal_browser_model_, event_manager_);
        }
    }

    context_menu_->addEvent(signal_event_->getId());
    context_menu_mutex_.unlock();
}

}
