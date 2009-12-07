#include "signal_graphics_item.h"
#include "event_graphics_item.h"
#include "signal_browser_view.h"
#include "signal_browser_model_4.h"
#include "new_event_undo_command.h"
#include "y_axis_widget_4.h"
#include "../command_stack.h"
#include "../event_color_manager.h"
#include "../main_window_model.h"
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

#include <cmath>

namespace BioSig_
{

// prefered pixel per sample
float64 SignalGraphicsItem::prefered_pixel_per_sample_ = 1.0;


//-----------------------------------------------------------------------------
SignalGraphicsItem::SignalGraphicsItem(SignalBuffer& buffer, const SignalChannel& channel,
                     SignalBrowserModel& model, SignalBrowserView* browser)
: signal_buffer_(buffer),
  signal_channel_(channel),
  signal_browser_model_(model),
  signal_browser_(browser),
  minimum_(channel.getPhysicalMinimum()),
  maximum_(channel.getPhysicalMaximum()),
  y_zoom_(1),
  y_grid_pixel_intervall_(0),
  draw_y_grid_ (true),
  y_offset_((channel.getPhysicalMinimum() +
             channel.getPhysicalMaximum()) / 2.0),
  height_ (100), // FIXME: arbitrary number!!
  shifting_ (false),
  new_event_ (false),
  created_event_item_ (0),
  hand_tool_on_ (false)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
    // nothing to do
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
        int32 width = (int32)(signal_buffer_.getBlockDuration() *
                          signal_buffer_.getNumberBlocks() * signal_browser_model_.getPixelPerSec());

    // std::cout << "SignalGraphicsItem::boundingRect width = " << width << std::endl;
    // std::cout << "SignalGraphicsItem::boundingRect top_margin_ = " << top_margin_ << std::endl;

    return QRectF (0, 0, width, height_);
}

//-----------------------------------------------------------------------------
// get maximum
float64 SignalGraphicsItem::getMaximum()
{
    return maximum_;
}

//-----------------------------------------------------------------------------
// get minimum
float64 SignalGraphicsItem::getMinimum()
{
    return minimum_;
}

//-----------------------------------------------------------------------------
float64 SignalGraphicsItem::getYZoom()
{
    return y_zoom_;
}

//-----------------------------------------------------------------------------
float64 SignalGraphicsItem::getYOffset()
{
    return y_offset_;
}

//-----------------------------------------------------------------------------
float64 SignalGraphicsItem::getYGridPixelIntervall()
{
    return y_grid_pixel_intervall_;
}

//-----------------------------------------------------------------------------
// zoom in
void SignalGraphicsItem::zoomIn()
{
    y_zoom_ *= 2.0;
}

//-----------------------------------------------------------------------------
// zoom out
void SignalGraphicsItem::zoomOut()
{
    y_zoom_ /= 2.0;
}

//-----------------------------------------------------------------------------
// auto scale
void SignalGraphicsItem::autoScale(ScaleMode auto_zoom_type)
{
    float64 min = signal_buffer_.getMinValue(signal_channel_.getNumber());
    float64 max = signal_buffer_.getMaxValue(signal_channel_.getNumber());

    if (auto_zoom_type == MAX_TO_MAX)
    {
        if (max > -min)
            min = -max;
        else
            max = -min;

        y_zoom_ = ((maximum_ - minimum_) / (max - min));
        y_offset_ = 0; // zero line in the middle
    }
    else
    {
        float64 abs_max = ((max < 0) ? (-max) : (max));
        float64 abs_min = ((min < 0) ? (-min) : (min));

        y_zoom_ = ((maximum_ - minimum_) / (abs_max + abs_min));
        y_offset_ = ((max + min) / 2.0);
    }
}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::paint (QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    painter->drawRect(boundingRect());
    if (new_event_)
        painter->fillRect(new_signal_event_->getPosition(), 0, new_signal_event_->getDuration(), height_, new_event_color_);
    QRectF clip (option->exposedRect);
    painter->setClipping(true);
    painter->setClipRect(clip);
    // clip.setWidth(clip.width()*2);
    signal_buffer_.setChannelActive(signal_channel_.getNumber(), true);

    int32 draw_start_x = clip.x(); //- 1;
    int32 draw_end_x = draw_start_x + clip.width(); //+ 1;

    int32 item_height = height_;
//    float64 item_y = y();

    float64 block_duration = signal_buffer_.getBlockDuration();
    float64 pixel_per_sec = signal_browser_model_.getPixelPerSec();

    // subsampling
    float64 samples_per_sec =  signal_channel_.getSamplesPerRecord() *
                               signal_buffer_.getRecordsPerBlock() /
                               block_duration;

    uint32 sub_sampling_type = SignalBuffer::NO_SUBSAMPLING;

   while (samples_per_sec / 2.0 * prefered_pixel_per_sample_ >= pixel_per_sec
           && sub_sampling_type < signal_buffer_.getMaxSubsampling())
    {
        block_duration *= 2.0;
        samples_per_sec /= 2.0;
        sub_sampling_type++;
    }

    // block range
    float64 tmp = draw_start_x / (block_duration * pixel_per_sec);
    uint32 first_block = (uint32)tmp;
    tmp = draw_end_x / (block_duration * pixel_per_sec);
    uint32 last_block = (uint32)tmp;

    float64 x_float = first_block * block_duration * pixel_per_sec;
    float64 x_float_inc = pixel_per_sec / samples_per_sec;

    // std::cout << "x_float = " << x_float <<  "; x_float_inc = " << x_float_inc << std::endl;


    // y range
    float64 value_range = (maximum_ - minimum_) / y_zoom_;
    float64 upper_value = y_offset_ + value_range / 2.0;


    //painter->setClipping(true);
    //painter->setClipRect(draw_start_x, draw_start_y, clip.width(),
    //              clip.height()); //, QPainter::CoordPainter);

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
}



//-----------------------------------------------------------------------------
// get range from buffer
void SignalGraphicsItem::getRangeFromBuffer(float64 factor)
{
    minimum_ = round125(signal_buffer_.getMinValue(signal_channel_.getNumber()) * factor);
    maximum_ = round125(signal_buffer_.getMaxValue(signal_channel_.getNumber()) * factor);
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::updateYGridIntervall()
{
    float64 value_range = (maximum_ - minimum_) / y_zoom_;
    float64 item_height = this->boundingRect().height();
    float64 y_grid_intervall
        = round125(value_range / item_height *
                   signal_browser_model_.getPreferedYGirdPixelIntervall());
    y_grid_pixel_intervall_ = item_height * y_grid_intervall / value_range;
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
void SignalGraphicsItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    QPoint p = event->screenPos();
    if (shifting_)
    {
        int32 dy = p.y() - move_start_point_.y();
        move_start_point_ = p;
        move_start_point_ = p;
        float64 range = (maximum_ - minimum_) / y_zoom_;
        y_offset_ = y_offset_ + dy * range / height_;
        update();
        signal_browser_->updateWidgets(false);
    }
    else if (new_event_)
    {
        float old_width = new_signal_event_->getDuration();
        float new_event_width = event->scenePos().x() - new_signal_event_->getPosition();
        if (new_event_width < 0)
            new_event_width = 0;
        update(new_signal_event_->getPosition(), 0,
                     new_event_width > old_width ? new_event_width : old_width,
                     height_);
        new_signal_event_->setDuration(new_event_width);
    }
    else
        event->ignore();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();

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


            if (signal_browser_model_.getActualEventCreationType() == static_cast<uint16>(-1))
            {
                signal_browser_model_.changeSelectedEventType();
                break;
            }

            new_event_ = true;
            new_signal_event_ = QSharedPointer<SignalEvent>(new SignalEvent(event->scenePos().x(), signal_browser_model_.getActualEventCreationType(), signal_buffer_.getEventSamplerate(),
                                                                            signal_channel_.getNumber(), 0));
            new_event_color_ = signal_browser_model_.getMainWindowModel().getEventColorManager().getEventColor(signal_browser_model_.getActualEventCreationType());
            break;
        }
            break;
        default:
            event->ignore();
    }
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    if (hand_tool_on_)
        event->ignore();

    if (new_event_)
    {
        NewEventUndoCommand* new_event_command = new NewEventUndoCommand(signal_browser_model_, new_signal_event_, signal_buffer_.getEventSamplerate() / signal_browser_model_.getPixelPerSec());
        CommandStack::instance().executeEditCommand (new_event_command);
    }

    shifting_ = false;
    hand_tool_on_ = false;
    new_event_ = false;
    setCursor(QCursor());
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::contextMenuEvent (QGraphicsSceneContextMenuEvent * event)
{
    event->accept();
    EventGraphicsItem::displayContextMenu(event);
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::drawYAxis (QPainter * painter, const QStyleOptionGraphicsItem * option)
{
    QRectF clip (option->exposedRect);
    int32 y_start = clip.y();
    int32 y_end = y_start + clip.height();
    int32 w = 100;
    int32 x_end = clip.x() + w;

    painter->setPen(Qt::black);
    painter->drawLine(x_end - 1, y_start, x_end -1, y_end);
/*
    p.setPen(Qt::black);
    float64 float_y_start = floor(y_start / intervall) * intervall;
    float64 float_y_end = ceil(y_end / intervall) * intervall;
    QMap<int32, SignalCanvasItem*>::iterator iter = channel_nr2signal_canvas_item_.begin();

    for (float32 float_item_y = 0;
         float_item_y < float_y_end && iter != channel_nr2signal_canvas_item_.end();
         float_item_y += intervall, iter++)
    {
        if (float_item_y > float_y_start - intervall)
        {
            float64 value_range = (iter.data()->getMaximum() - iter.data()->getMinimum()) /
                                  iter.data()->getYZoom();

            float64 upper_value = iter.data()->getYOffset() + value_range / 2.0;

            p.setClipRect(0, (int32)float_item_y, w, intervall);
//            p.setPen(Qt::black);
            p.drawLine(0, (int32)float_item_y + signal_height,
                       w - 1, (int32)float_item_y + signal_height);

            // draw y markers and values
            float64 y_grid_pixel_intervall = iter.data()->getYGridPixelIntervall();
            float64 y_grid_intervall = y_grid_pixel_intervall / signal_height * value_range;

            float64 value = (int32)((upper_value + y_grid_intervall) / y_grid_intervall) * y_grid_intervall;
            float64 y_float = (upper_value - value) * signal_height / value_range + float_item_y;

            for (;
                 value > upper_value - value_range - y_grid_intervall;
                 value -= y_grid_intervall, y_float += y_grid_pixel_intervall)
            {
                int32 y = (int32)(y_float + 0.5);
                p.drawLine(w - 5, y, w - 1, y);
                p.drawText(0, (int32)(y - 20) , w - 10, 40,
                       Qt::AlignRight | Qt::AlignVCenter, QString("%1")
                                                                .arg(qRound(value * 100) / 100.0));
            }
        }
    }*/
}


}
