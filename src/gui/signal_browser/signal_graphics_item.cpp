// © SigViewer developers
//
// License: GPL-3.0


#include "signal_graphics_item.h"
#include "signal_browser_model_4.h"
#include "editing_commands/new_event_undo_command.h"
#include "base/math_utils.h"
#include "gui/signal_browser_mouse_handling.h"
#include "gui/gui_action_factory.h"
#include "file_handling/xdf_reader.h"

#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QToolTip>
#include <QSettings>
#include <QApplication>
#include <QPalette>
#include <cmath>
#include <limits>


namespace sigviewer
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
  y_offset_ (0),
  height_ (signal_view_settings->getChannelHeight()),
  width_ (0),
  shifting_ (false),
  new_event_ (false),
  created_event_item_ (0),
  hand_tool_on_ (false),
  grid_color_ (QColor(220, 220, 220, 50)),   //set the default grid color
  label_color_ (QApplication::palette().color(QPalette::WindowText))
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
    setAcceptHoverEvents(false);
    connect(signal_view_settings.data(), SIGNAL(gridFragmentationChanged()), SLOT(updateYGridIntervall()));
    connect(signal_view_settings.data(), SIGNAL(enableXGrid(bool)), SLOT(toggleXGrid(bool)));
    connect(signal_view_settings.data(), SIGNAL(enableYGrid(bool)), SLOT(toggleYGrid(bool)));
    connect(signal_view_settings.data(), SIGNAL(separatorEnabled(bool)), SLOT(enableSeparator(bool)));
    connect(signal_view_settings.data(), SIGNAL(gridColorChanged(QColor)), SLOT(updateGridColor(QColor)));
    connect(signal_view_settings.data(), SIGNAL(labelColorChanged(QColor)), SLOT(updateLabelColor(QColor)));

    QSettings settings;

    settings.beginGroup("SignalBrowserModel");
    draw_x_grid_ = settings.value("show_x_grid", false).toBool();
    draw_y_grid_ = settings.value("show_y_grid", false).toBool();
    draw_separator = settings.value("show_separator", false).toBool();
    settings.endGroup();
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
void SignalGraphicsItem::toggleXGrid(bool enable)
{
    draw_x_grid_ = enable;
    update ();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::toggleYGrid(bool enable)
{
    draw_y_grid_ = enable;
    update ();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::enableSeparator(bool enable)
{
    draw_separator = enable;
    update();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::updateGridColor(QColor gridColor)
{
    grid_color_ = gridColor;
    update();
}

void SignalGraphicsItem::updateLabelColor(QColor labelColor)
{
    label_color_ = labelColor;
    update();
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
    // Schedule a repaint of this item only — do NOT call
    // signal_browser_model_.update() here as that re-layouts the entire scene
    // for every channel in a multi-channel scale loop (O(N²)).
    update ();
}

//-----------------------------------------------------------------------------
void SignalGraphicsItem::paint (QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    if (option->exposedRect.width() < 1)
        return;

    bool channel_overlapping = signal_view_settings_->getChannelOverlapping();

    if (draw_separator && !channel_overlapping)
        //        painter->drawRect(boundingRect());
    {
        //draw only upper and lower border, no verdical borders
        painter->setPen(label_color_);
        painter->drawLine(0, 0, width_, 0);
        painter->drawLine(0, height_, width_, height_);
    }

    if (new_event_)
        painter->fillRect(new_signal_event_->getPosition(), 0, new_signal_event_->getDuration(), height_, new_event_color_);

    QRectF clip (option->exposedRect);

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

    if (draw_x_grid_)
        drawXGrid (painter, option);

    if (draw_y_grid_)
        drawYGrid (painter, option);

    //You can't draw the lines here because it's after the clipping, thus the lower border is always
    //overwritten by the next channel, and the lower border of the last channel is never shown
//    if (draw_separator && !channel_overlapping)
//    {
//        painter->setPen(label_color_);
//        painter->drawLine(0, 0, width_, 0);
//        painter->drawLine(0, height_, width_, height_);
//    }

    painter->translate (0, height_ / 2.0f);

    // Clip all signal drawing to the channel's vertical extent. When y_zoom_ is
    // large (zoomed in on amplitude), sample values outside the visible range map
    // to screen coordinates of millions of pixels. Without an explicit clip rect,
    // QPainter's rasterizer receives these extreme values and must process them
    // entirely — its internal 26.6 fixed-point arithmetic can overflow, and even
    // short of overflow the clipping work for huge lines is significant. Setting
    // the clip rect here lets Qt efficiently discard out-of-range geometry and
    // compute precise entry/exit intersections for lines that cross the boundary.
    const double YMIN = -(double)height_ * 0.5;
    const double YMAX =  (double)height_ * 0.5;
    // Y_OVERDRAW: allow points this far outside the clip boundary. Qt uses the
    // overdraw region to compute correct clip-intersection geometry, so the
    // rendered result at the clip edge is exact even though the true sample
    // coordinate may be orders of magnitude further away.
    const double Y_OVERDRAW = (double)height_;
    painter->setClipRect(QRectF(clip.x(), YMIN, clip.width(), (double)height_));

    painter->setPen (color_manager_->getChannelColor (id_));

    if (pixel_per_sample >= 1.0)
    {
        // Full-resolution path: build a polyline per contiguous (non-NaN) segment.
        //
        // When the signal is zoomed in vertically, many consecutive samples share
        // the same clamped OOB Y value (all above or all below the visible range).
        // Drawing all such points is wasteful — a run of N identical-Y points is
        // just a horizontal line segment that only needs its first and last endpoints.
        // We collapse each OOB run down to at most 2 points, keeping the polygon
        // size proportional to the number of vertically-visible samples rather than
        // the total number of samples in the horizontal clip window.
        QPolygonF points;
        points.reserve(static_cast<int>(data_block->size()));

        const double oob_above_y = YMIN - Y_OVERDRAW;  // clamped Y for above-range samples
        const double oob_below_y = YMAX + Y_OVERDRAW;  // clamped Y for below-range samples

        // State for the current OOB run.
        bool   in_oob_run    = false;
        double oob_run_y     = 0.0;  // clamped Y of the active OOB run
        double oob_run_end_x = 0.0;  // X of the most-recent sample in this run

        // Emit the pending OOB run end-point (if different from the run start).
        // Called whenever we leave an OOB run: back in-range, direction change, or NaN.
        auto flush_oob_endpoint = [&]() {
            if (in_oob_run) {
                if (!points.empty() && points.last().x() != oob_run_end_x)
                    points.append(QPointF(oob_run_end_x, oob_run_y));
                in_oob_run = false;
            }
        };
        auto flush_segment = [&]() {
            flush_oob_endpoint();
            if (points.size() > 1)
                painter->drawPolyline(points);
            else if (points.size() == 1)
                painter->drawPoint(points.first());
            points.clear();
        };

        for (int index = 0; index < static_cast<int>(data_block->size()); index++)
        {
            float64 y = (*data_block)[index];
            if (std::isnan(y))
            {
                flush_segment();
            }
            else
            {
                double raw_y = y_offset_ - y_zoom_ * y;
                double clamped_y;
                if (raw_y < oob_above_y)       clamped_y = oob_above_y;
                else if (raw_y > oob_below_y)  clamped_y = oob_below_y;
                else                           clamped_y = raw_y;

                const bool is_oob = (raw_y < oob_above_y || raw_y > oob_below_y);

                if (is_oob && in_oob_run && clamped_y == oob_run_y)
                {
                    // Continuing the same OOB run: just slide the pending end-point.
                    oob_run_end_x = last_x;
                }
                else if (is_oob)
                {
                    // New OOB run (or direction change): close the previous OOB run
                    // and open a new one, emitting the first point immediately.
                    flush_oob_endpoint();
                    points.append(QPointF(last_x, clamped_y));
                    in_oob_run    = true;
                    oob_run_y     = clamped_y;
                    oob_run_end_x = last_x;
                }
                else
                {
                    // In-range point: close any OOB run first (emits its end-point),
                    // then add this point normally.
                    flush_oob_endpoint();
                    points.append(QPointF(last_x, clamped_y));
                }
            }
            last_x += pixel_per_sample;
        }
        flush_segment();
    }
    else
    {
        // Min-max envelope path: draw one vertical line per screen pixel column.
        //
        // When pre-computed downsampled data is available (built on file open)
        // each pixel inspects O(1) downsampled entries instead of O(N) raw
        // samples, making per-paint cost independent of zoom level.
        int samples_per_pixel = static_cast<int>(std::ceil(1.0 / pixel_per_sample));
        unsigned ds_factor = channel_manager_.getNearestDownsamplingFactor (
                                 id_, static_cast<unsigned>(samples_per_pixel));

        QSharedPointer<DataBlock const> ds_min, ds_max;
        if (ds_factor > 1)
        {
            ds_min = channel_manager_.getDownsampledMin (id_, ds_factor);
            ds_max = channel_manager_.getDownsampledMax (id_, ds_factor);
        }

        int first_pixel = static_cast<int>(std::floor(clip.x()));
        int last_pixel  = static_cast<int>(std::ceil(clip.x() + clip.width()));

        // Per-pixel envelope values, used to connect adjacent bars with a
        // diagonal line so steep transitions don't leave visible gaps.
        float32 prev_mid = std::numeric_limits<float32>::quiet_NaN();

        for (int px = first_pixel; px <= last_pixel; px++)
        {
            float32 ymin = 0.0f, ymax = 0.0f;
            bool has_valid = false;

            if (ds_min && ds_max)
            {
                // Convert this pixel column's screen-x range to absolute raw
                // sample indices, then to downsampled indices.
                double raw_start = static_cast<double>(px) / pixel_per_sample;
                double raw_end   = static_cast<double>(px + 1) / pixel_per_sample;
                int di_start = static_cast<int>(std::floor(raw_start / ds_factor));
                int di_end   = static_cast<int>(std::ceil (raw_end   / ds_factor));
                di_start = std::max(di_start, 0);
                di_end   = std::min(di_end, static_cast<int>(ds_min->size()));

                for (int di = di_start; di < di_end; di++)
                {
                    float32 lo = (*ds_min)[di];
                    float32 hi = (*ds_max)[di];
                    if (!std::isnan(lo))
                    {
                        if (!has_valid)
                        {
                            ymin = lo; ymax = hi;
                            has_valid = true;
                        }
                        else
                        {
                            if (lo < ymin) ymin = lo;
                            if (hi > ymax) ymax = hi;
                        }
                    }
                }
            }
            else
            {
                // On-the-fly fallback when pre-computed data is not available.
                int j_start = static_cast<int>(std::floor(static_cast<double>(px) / pixel_per_sample))
                              - static_cast<int>(start_sample);
                int j_end   = static_cast<int>(std::ceil(static_cast<double>(px + 1) / pixel_per_sample))
                              - static_cast<int>(start_sample);
                j_start = std::max(j_start, 0);
                j_end   = std::min(j_end, static_cast<int>(data_block->size()));

                for (int j = j_start; j < j_end; j++)
                {
                    float32 v = (*data_block)[j];
                    if (!std::isnan(v))
                    {
                        if (!has_valid) { ymin = ymax = v; has_valid = true; }
                        else
                        {
                            if (v < ymin) ymin = v;
                            if (v > ymax) ymax = v;
                        }
                    }
                }
            }

            if (has_valid)
            {
                float32 cur_mid = (ymin + ymax) * 0.5f;
                double screen_x = px + 0.5;
                double sy_max = y_offset_ - y_zoom_ * ymax;
                double sy_min = y_offset_ - y_zoom_ * ymin;

                // sy_max is the screen-Y of the amplitude maximum (top of bar,
                // smaller Y value); sy_min is the screen-Y of the amplitude
                // minimum (bottom of bar, larger Y value).
                // Skip bars that lie entirely outside the visible vertical range.
                if (sy_min >= YMIN && sy_max <= YMAX)
                {
                    // Clamp the bar endpoints to the visible range. For a vertical
                    // bar this is exact: we're simply showing the portion of the
                    // amplitude range that falls within the display window.
                    sy_max = std::max(sy_max, YMIN);
                    sy_min = std::min(sy_min, YMAX);

                    // Draw a diagonal connector from the midpoint of the previous
                    // bar to the midpoint of this bar. This fills any vertical gap
                    // between adjacent non-overlapping bars without inflating the
                    // shown amplitude range.
                    if (!std::isnan(prev_mid))
                    {
                        double prev_sy = std::max(YMIN, std::min(YMAX, y_offset_ - y_zoom_ * (double)prev_mid));
                        double cur_sy  = std::max(YMIN, std::min(YMAX, y_offset_ - y_zoom_ * (double)cur_mid));
                        painter->drawLine(QPointF(screen_x - 1.0, prev_sy),
                                          QPointF(screen_x,       cur_sy));
                    }

                    // Overdraw the precise min–max bar on top.
                    painter->drawLine(QPointF(screen_x, sy_max),
                                      QPointF(screen_x, sy_min));
                }

                prev_mid = cur_mid;
            }
            else
            {
                prev_mid = std::numeric_limits<float32>::quiet_NaN();
            }
        }
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
        signal_browser_model_.update();
        emit shifting (id_);
    }
    else if (new_event_)
    {
        float64 pixel_per_sample = signal_view_settings_->getPixelsPerSample();
        int32 sample_cleaned_pos = event->scenePos().x() / pixel_per_sample + 0.5;
        sample_cleaned_pos *= pixel_per_sample;
        int32 new_event_width = new_signal_event_->getDuration ();
        size_t old_pos = new_signal_event_->getPosition ();
        size_t old_width = new_signal_event_->getDuration ();

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
    for (const auto event : events)
    {
        QSharedPointer<SignalEvent const> signal_event = event_manager_->getEvent(event);
        if (shown_types.count(signal_event->getType()))
        {
            if (event_string.size())
                event_string += "<br /><br />";
            event_string += tr("<b>%1</b><br />Start: %2s; Duration: %3s").arg(event_manager_->getNameOfEvent (event)).arg(signal_event->getPositionInSec()).arg(signal_event->getDurationInSec());
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

            if (event_manager_->getFileType().startsWith("XDF", Qt::CaseInsensitive))
            {
                //If user added events in Sigviewer, we will create a new stream to store these events
                //and later store back to the XDF file
                if (!XDFdata->userAddedStream)
                {
                    //check whether a user added stream has already been existing
                    XDFdata->userAddedStream = XDFdata->streams.size();
                    XDFdata->streams.emplace_back();
                    time_t currentTime = time(nullptr);
                    std::string timeString = asctime(localtime(&currentTime));
                    timeString.pop_back(); //we don't need '\n' at the end
                    XDFdata->streams.back().streamHeader =
                            "<?xml version='1.0'?>"
                            "<info>"
                                "<name>User Created Event Stream</name>"
                                "<type>Events</type>"
                                "<channel_count>1</channel_count>"
                                "<nominal_srate>0</nominal_srate>"
                                "<channel_format>string</channel_format>"
                                "<source_id>User Added Events</source_id>"
                                "<version>1</version>"
                                "<created_at>" + timeString + "</created_at>"
                                "<uid/>"
                                "<session_id/>"
                                "<hostname/>"
                                "<desc />"
                            "</info>";
                }
                new_signal_event_ = QSharedPointer<SignalEvent>
                        (new SignalEvent(sample_cleaned_pos,
                                         signal_browser_model_.getActualEventCreationType(),
                                         event_manager_->getSampleRate(), XDFdata->userAddedStream,
                                         id_));
                //Add the newly created event to the XDFdata object for later use
                QString eventName = event_manager_->getNameOfEventType(signal_browser_model_.getActualEventCreationType());
                XDFdata->userCreatedEvents.emplace_back
                        (eventName.toStdString(), (sample_cleaned_pos/event_manager_->getSampleRate()) + XDFdata->minTS);
            }
            else
            {
                new_signal_event_ = QSharedPointer<SignalEvent>
                        (new SignalEvent(sample_cleaned_pos,
                                         signal_browser_model_.getActualEventCreationType(),
                                         event_manager_->getSampleRate(), -1,
                                         id_));
            }
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
    context_menu->addAction(GuiActionFactory::getInstance()->getQAction(tr("Change Color...")));
    context_menu->addAction(GuiActionFactory::getInstance()->getQAction(tr("Scale...")));
    if (signal_browser_model_.getShownChannels().size() > 1)
    {
        context_menu->addSeparator ();
        context_menu->addAction(GuiActionFactory::getInstance()->getQAction(tr("Hide Channel")));
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
    //painter->setPen (Qt::lightGray);
//    painter->setPen (QColor(220, 220, 220, 50)); // Qt::lightGray is still too dark and strong.
    painter->setPen(grid_color_);

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
    double pixel_per_sample = signal_view_settings_->getPixelsPerSample();
    double pixel_per_sec_ = pixel_per_sample * signal_view_settings_->getSampleRate();
    double interval_ = pixel_per_sec_ * MathUtils_::round125 (100.0 / pixel_per_sec_);
    if (interval_ < 1)
        return;

    QRectF clip (option->exposedRect);
//    painter->setPen (QColor(220, 220, 220, 50)); // Qt::lightGray is still too dark and strong.
    painter->setPen(grid_color_);


    if (clip.width() < 1)
        return;

    int32 x_start = clip.x();
    int32 x_end = clip.x() + clip.width();

    float64 float_x_start = floor((x_start + interval_ / 2) / interval_) *
                           interval_;

    float64 float_x_end = ceil((x_end - interval_ / 2) / interval_) *
                          interval_ + interval_ / 2;

    for (float32 float_x = float_x_start;
         float_x < float_x_end;
         float_x += interval_)
    {
        int32 x = (int32)(float_x + 0.5);
        painter->drawLine (x, 0, x, height_);
    }

//    for (int x = clip.x() - (static_cast<int>(clip.x()) % x_grid_interval_);
//         x < clip.x() + clip.width(); x += x_grid_interval_)
//        painter->drawLine (x, 0, x, height_);
}

}
