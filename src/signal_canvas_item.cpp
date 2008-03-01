// signal_canvas_item.cpp

#include "signal_canvas_item.h"
#include "smart_canvas/smart_canvas_view.h"
#include "base/signal_data_block.h"
#include "signal_browser.h"
#include "main_window.h"
#include "main_window_model.h"
#include "event_canvas_item.h"
#include "signal_browser_mouse_handling.h"
#include "base/math_utils.h"
#include "y_axis_widget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCursor>
#include <QContextMenuEvent>
#include <QSettings>

namespace BioSig_
{

// prefered pixel per sample
float64 SignalCanvasItem::prefered_pixel_per_sample_ = 1.0;

// constructor
SignalCanvasItem::SignalCanvasItem(SignalBuffer& buffer,
                                   const SignalChannel& channel,
                                   SignalBrowserModel& model,
                                   SignalBrowser* browser)
: SmartCanvasRectangle(browser->getCanvas()),
  signal_buffer_(buffer),
  signal_channel_(channel),
  signal_browser_model_(model),
  signal_browser_(browser),
  y_zoom_(1.0),
  y_offset_((channel.getPhysicalMinimum() +
             channel.getPhysicalMaximum()) / 2.0),
  shifting_(false),
  minimum_(channel.getPhysicalMinimum()),
  maximum_(channel.getPhysicalMaximum())
{
    // nothing
}

// get range from buffer
void SignalCanvasItem::getRangeFromBuffer(float64 factor)
{
    minimum_ = round125(signal_buffer_.getMinValue(signal_channel_.getNumber()) * factor);
    maximum_ = round125(signal_buffer_.getMaxValue(signal_channel_.getNumber()) * factor);
}

// rtti
int SignalCanvasItem::rtti() const
{
    return RTTI;
}

// load settings
void SignalCanvasItem::loadSettings()
{
    QSettings settings("SigViewer");
    prefered_pixel_per_sample_ = settings.value("SignalCanvasItem/prefered_pixel_per_sample",
                                                prefered_pixel_per_sample_).toDouble();
}

// save settings
void SignalCanvasItem::saveSettings()
{
    QSettings settings("SigViewer");
    settings.setValue("SignalCanvasItem/prefered_pixel_per_sample", prefered_pixel_per_sample_);
}

// update y grid intervall
void SignalCanvasItem::updateYGridIntervall()
{
    float64 value_range = (maximum_ - minimum_) / y_zoom_;
    float64 item_height = height();
    float64 y_grid_intervall
        = round125(value_range / item_height *
                   signal_browser_model_.getPreferedYGirdPixelIntervall());
    y_grid_pixel_intervall_ = item_height * y_grid_intervall / value_range;
}

// smart draw
void SignalCanvasItem::smartDraw(QPainter& p, const QRect& clip)
{
    signal_buffer_.setChannelActive(signal_channel_.getNumber(), true);

    int32 draw_start_x = clip.x() - 1;
    int32 draw_end_x = draw_start_x + clip.width() + 1;
    int32 item_height = height();
    float64 item_y = y();

    float64 block_duration = signal_buffer_.getBlockDuration();
    float64 pixel_per_sec = signal_browser_model_.getPixelPerSec();

    // subsampling
    float64 samples_per_sec =  signal_channel_.getSamplesPerRecord() *
                               signal_buffer_.getRecordsPerBlock() /
                               block_duration;
    uint32 sub_sampling_type = SignalBuffer::NO_SUBSAMPLING;
    while (samples_per_sec / 2.0 * prefered_pixel_per_sample_ >= pixel_per_sec
           && sub_sampling_type < SignalBuffer::MAX_SUBSAMPLING)
    {
        block_duration *= 2.0;
        samples_per_sec /= 2.0;
        sub_sampling_type++;
    }

    // block range
    float64 tmp = draw_start_x / block_duration / pixel_per_sec;
    uint32 first_block = (uint32)tmp;
    tmp = draw_end_x / block_duration / pixel_per_sec;
    uint32 last_block = (uint32)tmp;

    float64 x_float = first_block * block_duration * pixel_per_sec;
    float64 x_float_inc = pixel_per_sec / samples_per_sec;

    // y range
    float64 value_range = (maximum_ - minimum_) / y_zoom_;
    float64 upper_value = y_offset_ + value_range / 2.0;

    p.setClipRect(draw_start_x, (int32)item_y, clip.width() + 1,
                  item_height); //, QPainter::CoordPainter);

    // draw y grid
    float64 y_grid_intervall = y_grid_pixel_intervall_ / item_height * value_range;
    p.setPen(Qt::lightGray);
    for (float64 y_float = (upper_value - (int32)(upper_value / y_grid_intervall) * y_grid_intervall) *
                           item_height / value_range + item_y;
         y_float < item_y + item_height;
         y_float += y_grid_pixel_intervall_)
    {
        int32 y = (int32)(y_float +0.5);
        p.drawLine(draw_start_x, y, draw_end_x, y);
    }

    bool last_valid = false;
    int32 last_x = 0;
    int32 last_y = 0;
    p.setPen(Qt::darkBlue);
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
             index < data_block->number_samples;
             index++, buffer_valid++, buffer++, upper_buffer ++, lower_buffer++,
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
                                  (item_height - 1) + item_y + 0.5);
                if (last_valid)
                {
                    p.drawLine(last_x, last_y, x, y);
                }
                else
                {
                    p.drawPoint(x, y);
                }
                last_valid = true;
                last_y = y;
                last_x = x;
                if (sub_sampling_type > SignalBuffer::NO_SUBSAMPLING)
                {
                    // draw blocks
                    int32 y_u = (int32)((upper_value - *upper_buffer) /
                                        value_range * (item_height - 1) +
                                        item_y + 0.5);
                    int32 y_l = (int32)((upper_value - *lower_buffer) /
                                        value_range * (item_height - 1) +
                                        item_y + 0.5);
                    p.drawLine(x, y_u, x, y_l);
                }
            }
            else
            {
                last_valid = false;
            }
        }
    }

    p.setClipping(false); // !!!
}

// set y zoom
void SignalCanvasItem::setYZoom(float64 zoom)
{
    y_zoom_ = zoom < 1.0 ? 1.0 : 
              zoom > 16.0 ? 16.0 : zoom;
    float64 min_dif = (maximum_ - minimum_) / (2 * y_zoom_);
    y_offset_ = y_offset_ > maximum_ - min_dif ? maximum_ - min_dif :
                y_offset_ < minimum_ + min_dif ? minimum_ + min_dif :
                y_offset_;
    updateYGridIntervall();
}

// get y zoom
float64 SignalCanvasItem::getYZoom()
{
    return y_zoom_;
}

// set y offset
void SignalCanvasItem::setYOffset(float64 offset)
{
    float64 min_dif = (maximum_ - minimum_) / (2 * y_zoom_);
    y_offset_ = offset > maximum_ - min_dif ? maximum_ - min_dif :
                offset < minimum_ + min_dif ? minimum_ + min_dif :
                offset;
}

// get y offset
float64 SignalCanvasItem::getYOffset()
{
    return y_offset_;
}

// mouse press event
void SignalCanvasItem::mousePressEvent(QMouseEvent* e,
                                       Q3CanvasItemList& item_list, int32,
                                       SmartCanvasView* canvas_view)
{
    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    switch (SignalBrowserMouseHandling::getAction(e, mode))
    {
        case SignalBrowserMouseHandling::SHIFT_CHANNEL_ACTION:
            shifting_ = true;
            signal_browser_->getCanvasView()->viewport()
                                    ->setCursor(QCursor(Qt::SizeVerCursor));
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            move_start_point_ = e->globalPos();
            break;
        case SignalBrowserMouseHandling::NEW_EVENT_ACTION:
            {
/*
                // check if mouse press causes event edit
                for (Q3CanvasItemList::iterator it = item_list.begin();
                     it != item_list.end();
                     it++)
                {
                    if ((*it)->rtti() == EventCanvasItem::RTTI)
                    {
                        EventCanvasItem* event_item = (EventCanvasItem*)*it;
                        if (event_item->getMousePressAction(e, canvas_view) !=
                            EventCanvasItem::ACTION_NONE)
                        {
                            return; // event edit action
                        }
                    }
                }
*/
                // get shown events
                SignalBrowserModel::IntList shown_events;
                signal_browser_model_.getShownEventTypes(shown_events);
                if (shown_events.size() == 0)
                {
                    return; // no events shown
                }

                // add new event item
                SignalEvent new_event(0, 0);
                EventCanvasItem* selected_event_item
                    = signal_browser_model_.getSelectedEventItem();
                if (selected_event_item)
                {
                    SignalEvent* selected_event
                        = signal_buffer_.getEvent(selected_event_item->getId());
                    new_event.setType(selected_event->getType());
                    if (selected_event->getChannel() ==
                        SignalEvent::UNDEFINED_CHANNEL)
                    {
                        new_event.setChannel(SignalEvent::UNDEFINED_CHANNEL);
                    }
                    else
                    {
                        new_event.setChannel(signal_channel_.getNumber());
                    }
                }
                else
                {
                    new_event.setType(*shown_events.begin());
                    new_event.setChannel(signal_channel_.getNumber());
                }
                QPoint mouse_pos;
                mouse_pos = canvas_view->inverseWorldMatrix().map(e->pos());
                new_event.setPosition((int32)(mouse_pos.x() /
                                      signal_browser_model_.getPixelPerSec() *
                                      signal_buffer_.getEventSamplerate()));
                new_event.setDuration(0);
                EventCanvasItem* event_item
                    = signal_browser_model_.addEvent(new_event);

                // edit new event item
                signal_browser_model_.setSelectedEventItem(event_item);
                event_item->startMouseMoveEnd(canvas_view);
            }
            break;
        default:
            break;
    }
}

// mouse release event
void SignalCanvasItem::mouseReleaseEvent(QMouseEvent*, Q3CanvasItemList&,
                                         int32, SmartCanvasView* canvas_view)
{
    if (shifting_)
    {
        shifting_ = false;
        signal_browser_->getCanvasView()->viewport()
                                ->setCursor(QCursor(Qt::ArrowCursor));
        canvas_view->removeEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                         SmartCanvasView::MOUSE_MOVE_EVENT,
                                         this);
    }
}

// mouse move event
void SignalCanvasItem::mouseMoveEvent(QMouseEvent* e, Q3CanvasItemList&,
                                      SmartCanvasView*)
{
    if (shifting_)
    {
        QPoint p = e->globalPos();
        int32 dy = p.y() - move_start_point_.y();
        move_start_point_ = p;
        float64 range = (maximum_ - minimum_) / y_zoom_;
        setYOffset(y_offset_ + dy * range / height());
        canvas()->setChanged(rect());
        canvas()->update();
        signal_browser_->getYAxisWidget()->repaint();
    }
}

// wheel event
void SignalCanvasItem::wheelEvent(QWheelEvent* e, Q3CanvasItemList&,
                                      int32, SmartCanvasView*)
{
    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    switch (SignalBrowserMouseHandling::getAction(e, mode))
    {
        case SignalBrowserMouseHandling::ZOOM_CHANNEL_ACTION:
            if (e->delta() > 0)
            {
                zoomIn();
            }
            else
            {
                zoomOut();
            }
            break;
        default:
            break;
    }
}

// context menu event
void SignalCanvasItem::contextMenuEvent(QContextMenuEvent*, Q3CanvasItemList&,
                                        int32, Q3PopupMenu* menu,
                                        SmartCanvasView*)
{
    if (menu->count() > 0)
    {
        menu->insertSeparator();
    }
    QString label = signal_channel_.getLabel();
    menu->insertItem(QIcon(":/images/zoom_in_22x22.png"),
                     tr("Zoom In (%1)").arg(label), this, SLOT(zoomIn()));
    menu->insertItem(QIcon(":/images/zoom_out_22x22.png"),
                     tr("Zoom Out (%1)").arg(label), this, SLOT(zoomOut()));
    menu->insertItem(QIcon(":/images/auto_scale_22x22.png"),
                     tr("Auto Scale (%1)").arg(label), this, SLOT(autoScale()));
}

// zoom in
void SignalCanvasItem::zoomIn(bool update)
{
    setYZoom(y_zoom_ * 2.0);
    if (update)
    {
        canvas()->setChanged(rect());
        canvas()->update();
        signal_browser_->getYAxisWidget()->repaint();
    }
}

// zoom out
void SignalCanvasItem::zoomOut(bool update)
{
    setYZoom(y_zoom_ / 2.0);
    if (update)
    {
        canvas()->setChanged(rect());
        canvas()->update();
        signal_browser_->getYAxisWidget()->repaint();
    }
}

// auto scale
void SignalCanvasItem::autoScale(bool update)
{
    float64 min = signal_buffer_.getMinValue(signal_channel_.getNumber());
    float64 max = signal_buffer_.getMaxValue(signal_channel_.getNumber());
    setYZoom((maximum_ - minimum_) / (max - min));
    setYOffset(0); // zero line in the middle
    if (update)
    {
        canvas()->setChanged(rect());
        canvas()->update();
        signal_browser_->getYAxisWidget()->repaint();
    }
}

// get maximum
float64 SignalCanvasItem::getMaximum()
{
    return maximum_;

}

// get minimum
float64 SignalCanvasItem::getMinimum()
{
    return minimum_;
}

// get y grid pixel intervall
float64 SignalCanvasItem::getYGridPixelIntervall()
{
    return y_grid_pixel_intervall_;
}


} // namespace BioSig_
