#include "signal_graphics_item.h"
#include "event_graphics_item.h"
#include "signal_browser_model_4.h"
#include "editing_commands/new_event_undo_command.h"
#include "y_axis_widget_4.h"
#include "file_handling/channel_manager.h"
#include "command_executer.h"
#include "base/signal_event.h"
#include "base/signal_channel.h"
#include "base/math_utils.h"
#include "gui_impl/signal_browser_mouse_handling.h"
#include "gui/color_manager.h"
#include "gui/gui_action_factory.h"

#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPoint>
#include <QTime>
#include <QObject>
#include <QMenu>
#include <QPainter>
#include <QDebug>
#include <QToolTip>
#include <QSet>

#include <cmath>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
SignalGraphicsItem::SignalGraphicsItem (QSharedPointer<SignalViewSettings const> signal_view_settings,
                                        QSharedPointer<EventManager> event_manager,
                                        QSharedPointer<CommandExecuter> command_executor,
                                        ChannelManager const& channel_manager,
                                        QSharedPointer<ColorManager const> color_manager,
                                        ChannelID id,
                                        SignalBrowserModel& model)
: signal_view_settings_ (signal_view_settings),
  event_manager_ (event_manager),
  command_executor_ (command_executor),
  channel_manager_ (channel_manager),
  color_manager_ (color_manager),
  id_ (id),
  signal_browser_model_(model),
  minimum_ (channel_manager_.getMinValue (id_)),
  maximum_ (channel_manager_.getMaxValue (id_)),
  y_zoom_ (1),
  draw_y_grid_ (true),
  draw_x_grid_ (true),
  y_offset_ (0),
  height_ (signal_view_settings->getChannelHeight()),
  width_ (0),
  shifting_ (false),
  new_event_ (false),
  created_event_item_ (0),
  hand_tool_on_ (false)
{
#if QT_VERSION >= 0x040600
    setFlag (QGraphicsItem::ItemUsesExtendedStyleOption, true);
#endif
    setAcceptHoverEvents(false);
    connect (signal_view_settings.data(), SIGNAL(gridFragmentationChanged()), SLOT(updateYGridIntervall()));
}

//-----------------------------------------------------------------------------
SignalGraphicsItem::~SignalGraphicsItem ()
{

}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::setHeight (unsigned height)
{
   this->prepareGeometryChange();
   y_zoom_ = y_zoom_ * height / height_;
   y_offset_ = y_offset_* height / height_;
   height_ = height;
   width_ = channel_manager_.getNumberSamples() * signal_view_settings_->getPixelsPerSample();
   updateYGridIntervall ();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::setXGridInterval (unsigned interval)
{
    x_grid_interval_ = interval;
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
double SignalGraphicsItem::getValueRangeFragment() const
{
    return value_range_fragment_;
}


//-----------------------------------------------------------------------------
QString SignalGraphicsItem::getPhysicalDimensionString () const
{
    return channel_manager_.getChannelYUnitString (id_);
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::zoomIn()
{
    double mid_line_value = (maximum_ + minimum_) / 2;
    scaleImpl ((mid_line_value + minimum_) / 2.0, (maximum_ + mid_line_value) / 2.0);
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::zoomOut()
{
    double mid_line_value = (maximum_ + minimum_) / 2;
    scaleImpl ((2 * minimum_) - mid_line_value, (2 * maximum_) - mid_line_value);
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::scale (double lower_value, double upper_value)
{
    scaleImpl (lower_value, upper_value);
}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::autoScale (ScaleMode auto_zoom_type)
{

    minimum_ = channel_manager_.getMinValue (id_);
    maximum_ = channel_manager_.getMaxValue (id_);

    double max = maximum_;
    double min = minimum_;

    if (auto_zoom_type == MAX_TO_MAX)
    {
        if (fabs(max) >= fabs (min))
            min = max;
        else
            max = min;
        if (min > 0)
            min *= -1;
        if (max < 0)
            max *= -1;
    }
    scaleImpl (min, max);
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::scaleImpl (double min, double max)
{
    minimum_ = min;
    maximum_ = max;
    float64 new_y_zoom = height_ / (max - min);
    float64 new_y_offset = ((max + min) / 2) * new_y_zoom;
    if (y_zoom_ != new_y_zoom ||
        y_offset_ != new_y_offset)
    {
        y_zoom_ = new_y_zoom;
        y_offset_ = new_y_offset;
        updateYGridIntervall ();
    }
    update ();
}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::paint (QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    if (option->exposedRect.width() < 1)
        return;

    bool channel_overlapping = signal_view_settings_->getChannelOverlapping();

    if (!channel_overlapping)
        painter->drawRect(boundingRect());

    if (new_event_)
        painter->fillRect(new_signal_event_->getPosition(), 0, new_signal_event_->getDuration(), height_, new_event_color_);

    QRectF clip (option->exposedRect);

    if (!channel_overlapping)
    {
        painter->setClipping(true);
        painter->setClipRect(clip);
    }

    double pixel_per_sample = signal_view_settings_->getPixelsPerSample();

    double last_x = clip.x () - 10;
    if (last_x < 0)
        last_x = 0;
    unsigned start_sample = last_x / pixel_per_sample;
    if (start_sample > 0)
        start_sample--;

    unsigned length = ((clip.x() - start_sample * pixel_per_sample)
                       + clip.width() + 20);
    if (last_x + length > width_)
        length = width_ - last_x;

    length /= pixel_per_sample;
    if (length < channel_manager_.getNumberSamples() - start_sample)
        length++;


    QSharedPointer<DataBlock const> data_block = channel_manager_.getData (id_, start_sample, length);

    last_x = start_sample * pixel_per_sample;

    float32 last_y = (*data_block)[0];
    float32 new_y = 0;

    if (draw_x_grid_)
        drawXGrid (painter, option);
    painter->translate (0, height_ / 2.0f);
    if (draw_y_grid_)
        drawYGrid (painter, option);
    painter->setPen (color_manager_->getChannelColor (id_));

    for (int index = 0;
         index < static_cast<int>(data_block->size()) - 1;
         index++)
    {
        new_y = (*data_block)[index+1];
        painter->drawLine(last_x, y_offset_ - (y_zoom_ * last_y), last_x + pixel_per_sample, y_offset_ - (y_zoom_ * new_y));

        last_x += pixel_per_sample;
        last_y = new_y;
    }

    return;
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::updateYGridIntervall ()
{
    double value_range = (maximum_ - minimum_) ;/// y_zoom_;
    value_range_fragment_ = MathUtils_::round125 (value_range / signal_view_settings_->getGridFragmentation (Qt::Vertical));
    // qDebug () << "y_zoom_ = " << y_zoom_ << "; value_range_fragment_ =  " << value_range_fragment_;
    y_grid_pixel_intervall_ = static_cast<double>(height_) * value_range_fragment_ / value_range;
    emit updatedYGrid (id_);
    update ();
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
        float32 pixel_per_sample = signal_view_settings_->getPixelsPerSample ();
        int32 sample_cleaned_pos = event->scenePos().x() / pixel_per_sample + 0.5;
        sample_cleaned_pos *= pixel_per_sample;
        int32 new_event_width = new_signal_event_->getDuration ();
        uint32 old_pos = new_signal_event_->getPosition ();
        uint32 old_width = new_signal_event_->getDuration ();

        if (sample_cleaned_pos < new_signal_event_reference_x_)
        {
            new_event_width = new_signal_event_reference_x_ - sample_cleaned_pos;
            new_signal_event_->setPosition (sample_cleaned_pos);
        }
        else
        {
            new_signal_event_->setPosition (new_signal_event_reference_x_);
            new_event_width = sample_cleaned_pos - new_signal_event_->getPosition();
        }

        if (new_event_width < 0)
            new_event_width = 0;

        new_signal_event_->setDuration (new_event_width);

        int32 update_start = std::min(old_pos, new_signal_event_->getPosition());
        int32 update_end = std::max(old_pos + old_width, new_event_width + new_signal_event_->getPosition ());
        update (update_start, 0, update_end - update_start, height_);

        emit mouseAtSecond (sample_cleaned_pos / pixel_per_sample *
                                   channel_manager_.getSampleRate());
    }
    else
        event->ignore();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::hoverMoveEvent (QGraphicsSceneHoverEvent* event)
{
    unsigned sample_pos = event->scenePos().x() / signal_view_settings_->getPixelsPerSample ();

    emit mouseMoving (true);
    emit mouseAtSecond (sample_pos / channel_manager_.getSampleRate());

    if (event_manager_.isNull())
        return;

    std::set<EventID> events = event_manager_->getEventsAt (sample_pos, id_);
    std::set<EventType> shown_types = signal_browser_model_.getShownEventTypes();
    QString event_string;
    foreach (EventID event, events)
    {
        QSharedPointer<SignalEvent const> signal_event = event_manager_->getEvent(event);
        if (shown_types.count(signal_event->getType()))
        {
            if (event_string.size())
                event_string += "<br /><br />";
            event_string += "<b>" + event_manager_->getNameOfEvent (event) + "</b><br />";
            event_string += "Start: " + QString::number(signal_event->getPositionInSec()) + "s; ";
            event_string += "Duration: " + QString::number(signal_event->getDurationInSec()) + "s";
        }
    }

    if (event_string.size())
    {
        QToolTip::showText (event->screenPos(), event_string);
        setToolTip (event_string);
    }
    else
    {
        setToolTip ("");
        QToolTip::hideText();
    }
}


//-----------------------------------------------------------------------------
void SignalGraphicsItem::mousePressEvent (QGraphicsSceneMouseEvent * event )
{
    SignalVisualisationMode mode = signal_browser_model_.getMode();

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
            if (signal_browser_model_.getShownEventTypes ().size() == 0)
                break;

            float32 pixel_per_sample = signal_view_settings_->getPixelsPerSample ();
            int32 sample_cleaned_pos = event->scenePos().x() / pixel_per_sample + 0.5;
            sample_cleaned_pos *= pixel_per_sample;
            new_event_ = true;
            new_signal_event_ = QSharedPointer<SignalEvent>(new SignalEvent(sample_cleaned_pos,
                                                                            signal_browser_model_.getActualEventCreationType(),
                                                                            event_manager_->getSampleRate(),
                                                                            id_));
            new_event_color_ = color_manager_->getEventColor(signal_browser_model_.getActualEventCreationType());
            new_signal_event_reference_x_ = sample_cleaned_pos;
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
        NewEventUndoCommand* new_event_command = new NewEventUndoCommand (event_manager_, new_signal_event_, 1.0 / signal_view_settings_->getPixelsPerSample());
        command_executor_->executeCommand (new_event_command);
        signal_browser_model_.selectEvent (new_event_command->getIDOfNewlyCreatedEvent());
    }

    shifting_ = false;
    hand_tool_on_ = false;
    new_event_ = false;
    setCursor(QCursor());
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::contextMenuEvent (QGraphicsSceneContextMenuEvent * event)
{
    signal_browser_model_.selectChannel (id_);
    QMenu* context_menu = new QMenu (channel_manager_.getChannelLabel(id_));
    context_menu->addAction(GuiActionFactory::getInstance()->getQAction("Change Color..."));
    context_menu->addAction(GuiActionFactory::getInstance()->getQAction("Scale..."));
    if (signal_browser_model_.getShownChannels().size() > 1)
    {
        context_menu->addSeparator ();
        context_menu->addAction(GuiActionFactory::getInstance()->getQAction("Hide Channel"));
    }

    if (!EventGraphicsItem::displayContextMenu (event, context_menu))
        context_menu->exec(event->screenPos());
    else
        event->accept();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::wheelEvent (QGraphicsSceneWheelEvent* event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        if (event->delta() > 0)
            zoomIn();
        else if (event->delta() < 0)
            zoomOut();
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
    if (y_grid_pixel_intervall_ < 1)
        return;

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

//-----------------------------------------------------------------------------
void SignalGraphicsItem::drawXGrid (QPainter* painter,
                                    QStyleOptionGraphicsItem const* option)
{
    if (x_grid_interval_ < 1)
        return;

    QRectF clip (option->exposedRect);
    painter->setPen (Qt::lightGray);

    if (clip.width() < 1)
        return;

    for (int x = clip.x() - (static_cast<int>(clip.x()) % x_grid_interval_);
         x < clip.x() + clip.width(); x += x_grid_interval_)
        painter->drawLine (x, 0, x, height_);
}


}
