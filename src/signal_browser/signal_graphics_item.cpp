#include "signal_graphics_item.h"
#include "event_graphics_item.h"
#include "signal_browser_model_4.h"
#include "new_event_undo_command.h"
#include "y_axis_widget_4.h"
#include "../file_handling/channel_manager.h"
#include "../file_handling/event_manager.h"
#include "../command_executer.h"
#include "../base/signal_data_block.h"
#include "../base/signal_buffer.h"
#include "../base/signal_event.h"
#include "../base/signal_channel.h"
#include "../base/math_utils.h"
#include "../signal_browser_mouse_handling.h"

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
SignalGraphicsItem::SignalGraphicsItem(EventManager& event_manager,
                                       CommandExecuter& command_executor,
                                       ChannelManager& channel_manager,
                                       ChannelID id,
                                       SignalBuffer& buffer,
                                       const SignalChannel& channel,
                                       SignalBrowserModel& model)
: event_manager_ (event_manager),
  command_executor_ (command_executor),
  channel_manager_ (channel_manager),
  id_ (id),
  signal_buffer_(buffer),
  signal_channel_(channel),
  signal_browser_model_(model),
  minimum_ (channel_manager_.getMinValue (id_)),
  maximum_ (channel_manager_.getMaxValue (id_)),
  y_zoom_(1),
  y_grid_pixel_intervall_(1),
  draw_y_grid_ (true),
  y_offset_((channel.getPhysicalMinimum() +
             channel.getPhysicalMaximum()) / 2.0),
  height_ (1), // FIXME: arbitrary number!!
  shifting_ (false),
  new_event_ (false),
  created_event_item_ (0),
  hand_tool_on_ (false)
{
#if QT_VERSION >= 0x040600
    setFlag (QGraphicsItem::ItemUsesExtendedStyleOption, true);
#endif    
}

//-----------------------------------------------------------------------------
SignalGraphicsItem::~SignalGraphicsItem ()
{

}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::setHeight (int32 height)
{
   this->prepareGeometryChange();
   height_ = height;
}



//-----------------------------------------------------------------------------
QRectF SignalGraphicsItem::boundingRect () const
{
    // TODO: implement!
    int32 width = channel_manager_.getDurationInSec() * signal_browser_model_.getPixelPerXUnit();

    return QRectF (0, 0, width, height_);
}

//-----------------------------------------------------------------------------
// get maximum
float64 SignalGraphicsItem::getMaximum() const
{
    return maximum_;
}

//-----------------------------------------------------------------------------
// get minimum
float64 SignalGraphicsItem::getMinimum() const
{
    return minimum_;
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
    unsigned length = (clip.width() + 20.0f) / pixel_per_sample;

    QSharedPointer<DataBlock const> data_block = channel_manager_.getData (id_, start_sample, length);

    bool last_valid = false;
    float32 last_y = 0;
    float32 new_y = 0;

    painter->translate (0, height_ / 2.0f);  
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

    /// OLD IMPLEMENTATION:
    /*
    {

    int32 draw_start_x = clip.x() - 4; // some pixels before
    int32 draw_end_x = draw_start_x + clip.width() + 4;
    if (draw_start_x < 0)
        draw_start_x = 0;

    int32 item_height = height_;


    float64 block_duration = signal_buffer_.getBlockDuration();
    float64 pixel_per_sec = signal_browser_model_.getPixelPerXUnit();

    // subsampling
    float64 samples_per_sec =  signal_channel_.getSamplesPerRecord() *
                               signal_buffer_.getRecordsPerBlock() /
                               block_duration;

    uint32 sub_sampling_type = SignalBuffer::NO_SUBSAMPLING;

    // block range
    float64 tmp = draw_start_x / (block_duration * pixel_per_sec);
    uint32 first_block = (uint32)tmp;
    tmp = draw_end_x / (block_duration * pixel_per_sec);
    uint32 last_block = (uint32)tmp;

    float64 x_float = first_block * block_duration * pixel_per_sec;
    float64 x_float_inc = pixel_per_sec / samples_per_sec;

    // std::cout << "x_float = " << x_float <<  "; x_float_inc = " << x_float_inc << std::endl;


    // y range
    float64 value_range = (fabs(maximum_) + fabs(minimum_)) / y_zoom_;
    float64 upper_value = y_offset_ + value_range / 2.0;

    // draw y grid - begin
    if (draw_y_grid_)
    {
        float64 y_grid_intervall = y_grid_pixel_intervall_ / item_height * value_range;
        painter->setPen(Qt::lightGray);

        for (float64 y_float = (upper_value - (int32)(upper_value / y_grid_intervall) * y_grid_intervall) *
                                item_height / value_range;
                                y_float < + item_height;
                                y_float += y_grid_pixel_intervall_)
        {
            int32 y = (int32)(y_float +0.5);
            painter->drawLine(draw_start_x, y, draw_end_x, y);
        }
    }
    // draw y grid - end

    // draw x grid - begin
    if (draw_x_grid_)
    {
        float64 intervall = signal_browser_model_.getXGridPixelIntervall();

        if (intervall > 1)
        {
            int32 x_start = clip.x();
            int32 x_end = x_start + clip.width();

            float64 float_x_start = floor((x_start + intervall / 2) / intervall) *
                           intervall;

            float64 float_x_end = ceil((x_end - intervall / 2) / intervall) *
                          intervall + intervall / 2;

            painter->setPen(Qt::lightGray);
            for (float32 float_x = float_x_start;
                 float_x < float_x_end;
                 float_x += intervall)
            {
                int32 x = (int32)(float_x + 0.5);
                painter->drawLine(x, clip.y(), x, clip.y() + clip.height());
            }
        }
    }
    // draw x grid - end


    bool last_valid = false;
    int32 last_x = 0;
    int32 last_y = 0;

    painter->setPen(Qt::darkBlue);

    for (uint32 block_nr = first_block; block_nr <= last_block; block_nr++)
    {
        SignalDataBlock* data_block;
        data_block = signal_buffer_.getSignalDataBlock(
                                                    signal_channel_.getNumber(),
                                                    sub_sampling_type,
                                                    block_nr);

        if (!data_block)
        {
            continue; // error
        }

        float32* buffer = data_block->getBuffer();
        float32* upper_buffer = data_block->getUpperBuffer();
        float32* lower_buffer = data_block->getLowerBuffer();
        bool* buffer_valid = data_block->getBufferValid();

        for (uint32 index = 0;
             (index < data_block->number_samples) && (x_float < draw_end_x + x_float_inc);
             index++, buffer_valid++, buffer++, upper_buffer++, lower_buffer++,
             x_float+= x_float_inc)
        {
            if (x_float < draw_start_x - x_float_inc ||
                x_float > draw_end_x + x_float_inc)
            {
                continue; // out of drawing range
            }

            if (*buffer_valid)
            {
                int32 x = (int32)(x_float + 0.5);
                int32 y = (int32)((upper_value - *buffer) / value_range *
                                  (item_height - 1) + 0.5);

                if (last_valid)
                    painter->drawLine(last_x, last_y, x, y);
                else
                    painter->drawPoint(x, y);

                last_valid = true;
                last_y = y;
                last_x = x;

                if (sub_sampling_type > SignalBuffer::NO_SUBSAMPLING)
                {
                    // draw blocks
                    int32 y_u = (int32)((upper_value - *upper_buffer) /
                                        value_range * (item_height - 1) + 0.5);

                    int32 y_l = (int32)((upper_value - *lower_buffer) /
                                        value_range * (item_height - 1) +
                                        0.5);

                    painter->drawLine(x, y_u, x, y_l);
                }
            }
            else
            {
                last_valid = false;
            }
        }
    }

    // drawing the y axis
    // TODO: if drawing y-axis, dont draw the signal at this place!
    //drawYAxis(painter, option);


    //painter->setClipping(false); // !!!
    //if (time.elapsed() > 1)
    //{
    //    std::cout << "channel " << signal_channel_.getNumber() << "; clip.width() = " << clip.width() << "; clip.height() = " << clip.height() << std::endl;
    //    std::cout << "time passed: " << time.elapsed() << std::endl;
    //}
}*/
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::updateYGridIntervall ()
{
    float64 value_range = (fabs(maximum_) + fabs(minimum_));
    float64 y_grid_intervall
        = round125(value_range / height_ * signal_browser_model_.getPreferedYGirdPixelIntervall());
    y_grid_pixel_intervall_ = y_zoom_ * y_grid_intervall;
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
            new_event_color_ = signal_browser_model_.getEventColor(signal_browser_model_.getActualEventCreationType());
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
