#include "signal_graphics_item.h"
#include "event_graphics_item.h"
#include "signal_browser_model_4.h"
#include "new_event_undo_command.h"
#include "y_axis_widget_4.h"
#include "../file_handling/channel_manager.h"
#include "../file_handling/event_manager.h"
#include "../command_executer.h"
#include "../base/signal_event.h"
#include "../base/signal_channel.h"
#include "../base/math_utils.h"
#include "../signal_browser_mouse_handling.h"
#include "../application_context.h"
#include "../event_color_manager.h"

#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPoint>
#include <QTime>
#include <QObject>
#include <QMenu>
#include <QPainter>

#include <cmath>
#include <iostream>

namespace BioSig_
{

// prefered pixel per sample
float64 SignalGraphicsItem::prefered_pixel_per_sample_ = 1.0;


//-----------------------------------------------------------------------------
SignalGraphicsItem::SignalGraphicsItem (EventManager& event_manager,
                                        CommandExecuter& command_executor,
                                        ChannelManager& channel_manager,
                                        ChannelID id,
                                        const SignalChannel& channel,
                                        SignalBrowserModel& model)
: event_manager_ (event_manager),
  command_executor_ (command_executor),
  channel_manager_ (channel_manager),
  id_ (id),
  signal_channel_(channel),
  signal_browser_model_(model),
  minimum_ (channel_manager_.getMinValue (id_)),
  maximum_ (channel_manager_.getMaxValue (id_)),
  y_zoom_ (1),
  draw_y_grid_ (true),
  y_offset_ (0),
  height_ (model.getSignalHeight()),
  width_ (0),
  shifting_ (false),
  new_event_ (false),
  created_event_item_ (0),
  hand_tool_on_ (false)
{
#if QT_VERSION >= 0x040600
    setFlag (QGraphicsItem::ItemUsesExtendedStyleOption, true);
#endif    
    float64 value_range = (fabs(maximum_) + fabs(minimum_));
    float64 y_grid_intervall
        = round125(value_range / height_ * signal_browser_model_.getPreferedYGirdPixelIntervall());
    y_grid_pixel_intervall_ = y_zoom_ * y_grid_intervall;
}

//-----------------------------------------------------------------------------
SignalGraphicsItem::~SignalGraphicsItem ()
{

}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::setHeight (uint32 height)
{
   this->prepareGeometryChange();
   y_zoom_ = y_zoom_ * height / height_;
   y_offset_ = y_offset_* height / height_;
   height_ = height;
   width_ = channel_manager_.getDurationInSec() * signal_browser_model_.getPixelPerXUnit();
   updateYGridIntervall ();
}



//-----------------------------------------------------------------------------
QRectF SignalGraphicsItem::boundingRect () const
{
    return QRectF (0, 0, width_, height_);
}

//-----------------------------------------------------------------------------
float64 SignalGraphicsItem::getYZoom() const
{
    return y_zoom_;
}

//-----------------------------------------------------------------------------
float64 SignalGraphicsItem::getYOffset() const
{
    return y_offset_;
}

//-----------------------------------------------------------------------------
float64 SignalGraphicsItem::getYGridPixelIntervall() const
{
    return y_grid_pixel_intervall_;
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::zoomIn()
{
    y_zoom_ *= 2.0;
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::zoomOut()
{
    y_zoom_ /= 2.0;
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::autoScale (ScaleMode auto_zoom_type)
{
    float64 min = minimum_;
    float64 max = maximum_;
    float64 abs_max = ((max < 0) ? (-max) : (max));
    float64 abs_min = ((min < 0) ? (-min) : (min));

    if (auto_zoom_type == MAX_TO_MAX)
    {
        if (abs_max > abs_min)
            abs_min = abs_max;
        else
            abs_max = abs_min;
    }

    y_zoom_ = height_ / (abs_max + abs_min);
    y_offset_ = ((abs_max - abs_min) / 2) * y_zoom_;
    updateYGridIntervall ();
}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::paint (QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    painter->drawRect(boundingRect());
    if (new_event_)
    {
        painter->fillRect(new_signal_event_->getPosition(), 0, new_signal_event_->getDuration(), height_, new_event_color_);
    }
    QRectF clip (option->exposedRect);

    painter->setClipping(true);
    painter->setClipRect(clip);

    float64 pixel_per_sec = signal_browser_model_.getPixelPerXUnit();
    float32 pixel_per_sample = pixel_per_sec / channel_manager_.getSampleRate();

    // waldesel: better would be
    // pixel_per_sample = 1; (or some power of 2)

    float32 last_x = clip.x () - 10.0f;
    if (last_x < 0)
        last_x = 0;
    unsigned start_sample = last_x / pixel_per_sample;

    unsigned length = (clip.width() + 20.0f);
    if (last_x + length > width_)
        length = width_ - last_x;

    length /= pixel_per_sample;

    QSharedPointer<DataBlock const> data_block = channel_manager_.getData (id_, start_sample, length);

    bool last_valid = false;
    float32 last_y = 0;
    float32 new_y = 0;

    painter->translate (0, height_ / 2.0f);  
    if (draw_y_grid_)
        drawYGrid (painter, option);
    painter->setPen(Qt::darkBlue);

    for (unsigned index = 0;
         index < data_block->size();
         index++)
    {
        new_y = (*data_block)[index];
        if (last_valid)
            painter->drawLine(last_x, y_offset_ - (y_zoom_ * last_y), last_x + pixel_per_sample, y_offset_ - (y_zoom_ * new_y));
        else
            last_valid = true;

        last_x += pixel_per_sample;
        last_y = new_y;
    }
    return;
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::updateYGridIntervall ()
{
    float64 value_range = (fabs(maximum_) + fabs(minimum_));
    float64 y_grid_intervall
        = round125(value_range / height_ * signal_browser_model_.getPreferedYGirdPixelIntervall());
    y_grid_pixel_intervall_ = y_zoom_ * y_grid_intervall;

    emit updatedYGrid (id_);
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::enableYGrid(bool enabled)
{
    draw_y_grid_ = enabled;
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::enableXGrid(bool enabled)
{
    draw_x_grid_ = enabled;
}

//-----------------------------------------------------------------------------
QString const &SignalGraphicsItem::getLabel () const
{
    return signal_channel_.getLabel();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::mouseMoveEvent (QGraphicsSceneMouseEvent* event)
{
    QPoint p = event->screenPos();
    if (shifting_)
    {
        int32 dy = p.y() - move_start_point_.y();
        move_start_point_ = p;
        move_start_point_ = p;
        y_offset_ = y_offset_ + dy;
        update();
        emit shifting (id_);
    }
    else if (new_event_)
    {
        float old_width = new_signal_event_->getDuration();
        float pos = event->scenePos().x();
        if (pos < new_signal_event_->getPosition())
            pos = new_signal_event_->getPosition();
        float new_event_width = pos - new_signal_event_->getPosition();
        if (new_event_width < 0)
            new_event_width = 0;
        if (old_width < new_event_width)
            update (new_signal_event_->getPosition() + old_width, 0,
                    new_event_width - old_width, height_);
        else
            update (new_signal_event_->getPosition() + new_event_width, 0,
                    old_width - new_event_width, height_);

        new_signal_event_->setDuration(new_event_width);
        emit mouseAtSecond (pos / signal_browser_model_.getPixelPerXUnit());
    }
    else
        event->ignore();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::mousePressEvent (QGraphicsSceneMouseEvent * event )
{
    SignalBrowserMode mode = signal_browser_model_.getMode();

    switch (SignalBrowserMouseHandling::getAction(event, mode))
    {
        case SignalBrowserMouseHandling::HAND_SCROLL_ACTION:
            hand_tool_on_ = true;
            event->ignore();
            break;
        case SignalBrowserMouseHandling::SHIFT_CHANNEL_ACTION:
            shifting_ = true;

            setCursor(QCursor(Qt::SizeVerCursor));
            move_start_point_ = event->screenPos();
            break;
        case SignalBrowserMouseHandling::NEW_EVENT_ACTION:
            {             
            // get shown events
            SignalBrowserModel::IntList shown_events;
            signal_browser_model_.getShownEventTypes(shown_events);

            if (shown_events.size() == 0)
            {
                break; // no events shown
            }

            new_event_ = true;
            new_signal_event_ = QSharedPointer<SignalEvent>(new SignalEvent(event->scenePos().x(),
                                                                            signal_browser_model_.getActualEventCreationType(),
                                                                            event_manager_.getSampleRate(),
                                                                            id_,
                                                                            0));
            new_event_color_ = ApplicationContext::getInstance()->getEventColorManager()->getEventColor(signal_browser_model_.getActualEventCreationType());
            emit mouseMoving (true);
            break;
        }
            break;
        default:
            event->accept();
            if (!EventGraphicsItem::displaySelectionMenu (event))
                signal_browser_model_.unselectEvent ();
    }
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::mouseReleaseEvent (QGraphicsSceneMouseEvent* event)
{
    if (hand_tool_on_)
        event->ignore();

    if (new_event_)
    {
        emit mouseMoving (false);
        NewEventUndoCommand* new_event_command = new NewEventUndoCommand (event_manager_, new_signal_event_, event_manager_.getSampleRate() / signal_browser_model_.getPixelPerXUnit());
        command_executor_.executeCommand (new_event_command);
    }

    shifting_ = false;
    hand_tool_on_ = false;
    new_event_ = false;
    setCursor(QCursor());
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::contextMenuEvent (QGraphicsSceneContextMenuEvent * event)
{
    if (!EventGraphicsItem::displayContextMenu(event))
    {
        event->ignore();
        return; // TODO: implement!!!!
        QMenu context_menu;
        context_menu.addAction("Set Scaling");
        context_menu.addSeparator();
        context_menu.exec(event->screenPos());
    }
    else
        event->accept();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::wheelEvent (QGraphicsSceneWheelEvent* event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        if (event->delta() > 0)
            y_zoom_ *= event->delta() / 60;
        else if (event->delta() < 0)
            y_zoom_ /= -(event->delta()) / 60;
        update ();
        emit shifting (id_);
    }
    else if (event->modifiers().testFlag(Qt::ShiftModifier))
    {
        if (event->delta() > 0)
            signal_browser_model_.zoomInAll ();
        else if (event->delta() < 0)
            signal_browser_model_.zoomOutAll ();
    }
    else
        event->ignore();
}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::drawYGrid (QPainter* painter,
                                    QStyleOptionGraphicsItem const* option)
{
    QRectF clip (option->exposedRect);
    painter->setPen (Qt::lightGray);

    for (float64 y = y_offset_;
         y < height_ / 2;
         y += y_grid_pixel_intervall_)
    {
        if (y > -static_cast<int>(height_ / 2))
        {
            painter->drawLine (clip.x(), y, clip.x() + clip.width(), y);
        }
    }

    for (float64 y = y_offset_;
         y > -static_cast<int>(height_ / 2);
         y -= y_grid_pixel_intervall_)
    {
        if (y < height_ / 2)
        {
            painter->drawLine (clip.x(), y, clip.x() + clip.width(), y);
        }
    }

}


}
