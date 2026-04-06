// © SigViewer developers
//
// License: GPL-3.0

#include "overview_widget.h"
#include "base/data_block.h"
#include "base/signal_event.h"

#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSettings>

#include <cmath>
#include <limits>

namespace sigviewer
{

namespace
{
    constexpr int WIDGET_HEIGHT        = 120;
    constexpr int MIN_HEIGHT            = 40;
    constexpr int MAX_HEIGHT            = 600;
    constexpr int RESIZE_HANDLE_HEIGHT  = 5;
}

//-----------------------------------------------------------------------------
OverviewWidget::OverviewWidget(QSharedPointer<SignalVisualisationModel> model,
                               QSharedPointer<ColorManager const> color_manager,
                               QSharedPointer<EventManager const> event_manager,
                               QWidget* parent)
    : QWidget(parent),
      model_(model),
      color_manager_(color_manager),
      event_manager_(event_manager)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMouseTracking(true);
    const int saved_h = QSettings().value("MainWindow/overviewHeight", WIDGET_HEIGHT).toInt();
    setFixedHeight(qBound(MIN_HEIGHT, saved_h, MAX_HEIGHT));
}

//-----------------------------------------------------------------------------
QSize OverviewWidget::sizeHint() const        { return QSize(-1, WIDGET_HEIGHT); }
QSize OverviewWidget::minimumSizeHint() const { return QSize(0, MIN_HEIGHT); }

//-----------------------------------------------------------------------------
void OverviewWidget::updateOverview()      { update(); }
void OverviewWidget::setVisibleY(int32 y)  { current_visible_y_ = y; update(); }
void OverviewWidget::rebuildSignalImage()  { pixmap_dirty_ = true; update(); }

//-----------------------------------------------------------------------------
void OverviewWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    pixmap_dirty_ = true;
}

//-----------------------------------------------------------------------------
// Render all channels over the full time range into signal_pixmap_.
// Called once per data-source change; paintEvent only blits the result.
//-----------------------------------------------------------------------------
void OverviewWidget::renderSignals()
{
    const int w = width();
    const int h = height();
    signal_pixmap_ = QPixmap(w, h);
    signal_pixmap_.fill(palette().base().color());
    pixmap_dirty_ = false;

    if (!model_ || w <= 0 || h <= 0)
        return;

    const ChannelManager& cm         = model_->getChannelManager();
    const unsigned        total_samp = static_cast<unsigned>(cm.getNumberSamples());
    if (total_samp == 0)
        return;

    const auto shown = model_->getShownChannels();
    if (shown.empty())
        return;

    QPainter p(&signal_pixmap_);
    p.setRenderHint(QPainter::Antialiasing, false);

    const int n_ch  = static_cast<int>(shown.size());
    int       ch_idx = 0;

    for (ChannelID id : shown)
    {
        const int y_top = (ch_idx       * h) / n_ch;
        const int y_bot = ((ch_idx + 1) * h) / n_ch;
        const int ch_h  = y_bot - y_top;
        ++ch_idx;
        if (ch_h <= 0)
            continue;

        float norm_min = static_cast<float>(cm.getMinValue(id));
        float norm_max = static_cast<float>(cm.getMaxValue(id));
        if (norm_min >= norm_max) { norm_min -= 1.0f; norm_max += 1.0f; }
        const float range = norm_max - norm_min;

        QSharedPointer<DataBlock const> data = cm.getData(id, 0, total_samp);
        if (!data || data->size() == 0)
            continue;
        const size_t n = data->size();

        QColor ch_color = color_manager_ ? color_manager_->getChannelColor(id)
                                         : palette().text().color();
        p.setPen(ch_color);

        for (int x = 0; x < w; ++x)
        {
            const size_t s0  = static_cast<size_t>(static_cast<double>(x)     / w * n);
            const size_t s1r = static_cast<size_t>(static_cast<double>(x + 1) / w * n);
            const size_t s1  = std::min(s1r == s0 ? s0 + 1 : s1r, n);

            float mn =  std::numeric_limits<float>::max();
            float mx = -std::numeric_limits<float>::max();
            for (size_t s = s0; s < s1; ++s)
            {
                const float v = (*data)[s];
                if (!std::isnan(v)) { mn = std::min(mn, v); mx = std::max(mx, v); }
            }
            if (mn > mx) continue;

            const int y1 = qBound(y_top,
                y_bot - 1 - static_cast<int>((mx - norm_min) / range * (ch_h - 1)),
                y_bot - 1);
            const int y2 = qBound(y_top,
                y_bot - 1 - static_cast<int>((mn - norm_min) / range * (ch_h - 1)),
                y_bot - 1);
            p.drawLine(x, y1, x, y2);
        }
    }

    // Channel separator lines.
    p.setPen(QPen(palette().dark().color(), 1));
    for (int i = 1; i < n_ch; ++i)
        p.drawLine(0, (i * h) / n_ch, w - 1, (i * h) / n_ch);

    // Draw events on top of the signal traces.
    if (!event_manager_.isNull())
    {
        const auto shown      = model_->getShownChannels();
        const auto n_ch_drawn = static_cast<int>(shown.size());
        const auto shown_types = model_->getShownEventTypes();

        for (EventID id : event_manager_->getAllEvents())
        {
            QSharedPointer<SignalEvent const> ev = event_manager_->getEvent(id);
            if (!ev)
                continue;
            if (!shown_types.empty() && shown_types.count(ev->getType()) == 0)
                continue;

            // Horizontal extent.
            const unsigned pos = static_cast<unsigned>(ev->getPosition());
            const unsigned dur = static_cast<unsigned>(ev->getDuration());
            const int x1 = static_cast<int>(static_cast<double>(pos)           / total_samp * w);
            const int x2 = static_cast<int>(static_cast<double>(pos + (dur > 0 ? dur : 1)) / total_samp * w);
            const int ex  = qBound(0, x1, w - 1);
            const int ew  = qMax(1, qMin(x2, w) - ex);

            // Vertical extent: which channel row(s) to fill.
            const ChannelID ev_ch = ev->getChannel();
            int yt = 0, yb = h;  // default: all channels
            if (ev_ch != UNDEFINED_CHANNEL && n_ch_drawn > 0)
            {
                int row = 0;
                for (ChannelID c : shown)
                {
                    if (c == ev_ch) break;
                    ++row;
                }
                if (row < n_ch_drawn)
                {
                    yt = (row       * h) / n_ch_drawn;
                    yb = ((row + 1) * h) / n_ch_drawn;
                }
            }

            QColor color = color_manager_ ? color_manager_->getEventColor(ev->getType())
                                          : QColor(Qt::blue);
            color.setAlpha(80);
            p.fillRect(ex, yt, ew, yb - yt, color);
        }
    }
}

//-----------------------------------------------------------------------------
void OverviewWidget::paintEvent(QPaintEvent*)
{
    if (pixmap_dirty_ || signal_pixmap_.size() != size())
        renderSignals();

    QPainter painter(this);
    painter.drawPixmap(0, 0, signal_pixmap_);

    // Top border separator.
    painter.setPen(QPen(palette().dark().color(), 1));
    painter.drawLine(0, 0, width() - 1, 0);

    // Viewport highlight rectangle.
    const QRect vr = computeViewportRect();
    if (vr.isValid())
    {
        // Dim everything outside the viewport.
        QColor shade(0, 0, 0, 90);
        if (vr.left() > 0)
            painter.fillRect(0, 0, vr.left(), height(), shade);
        if (vr.right() < width() - 1)
            painter.fillRect(vr.right() + 1, 0, width() - vr.right() - 1, height(), shade);
        if (vr.top() > 0)
            painter.fillRect(vr.left(), 0, vr.width(), vr.top(), shade);
        if (vr.bottom() < height() - 1)
            painter.fillRect(vr.left(), vr.bottom() + 1, vr.width(), height() - vr.bottom() - 1, shade);

        // Bright border around the viewport rect.
        QColor border = palette().highlight().color();
        painter.setPen(QPen(border, 2));
        painter.drawRect(vr.adjusted(1, 1, -1, -1));
    }
}

//-----------------------------------------------------------------------------
QRect OverviewWidget::computeViewportRect() const
{
    if (!model_ || !model_->view())
        return QRect();

    const ChannelManager& cm         = model_->getChannelManager();
    unsigned              total_samp = static_cast<unsigned>(cm.getNumberSamples());
    if (total_samp == 0)
        return QRect();

    const int w = width();
    const int h = height();
    if (w <= 0 || h <= 0)
        return QRect();

    // --- Horizontal (time) ---
    unsigned shown_pos = model_->getShownPosition();
    int      vp_px     = model_->view()->getViewportWidth();
    float    main_pps  = model_->getSignalViewSettings()->getPixelsPerSample();
    if (main_pps <= 0.0f)
        return QRect();

    double viewport_samples = static_cast<double>(vp_px) / main_pps;
    double overview_pps     = static_cast<double>(w) / total_samp;

    int left_x = static_cast<int>(static_cast<double>(shown_pos) * overview_pps);
    int rect_w = qMax(2, static_cast<int>(viewport_samples * overview_pps));

    left_x = qBound(0, left_x, w - 1);
    if (left_x + rect_w > w)
        rect_w = w - left_x;
    rect_w = qMax(6, rect_w);   // always large enough to grab
    if (left_x + rect_w > w)
        left_x = qMax(0, w - rect_w);

    // --- Vertical (channels) ---
    auto shown_channels = model_->getShownChannels();
    int  n_ch           = static_cast<int>(shown_channels.size());
    int  top_y          = 0;
    int  rect_h         = h;
    if (n_ch > 0)
    {
        float ch_h_px = static_cast<float>(
            model_->getSignalViewSettings()->getChannelHeight());
        float overlap = model_->getSignalViewSettings()->getChannelOverlapping();
        float scene_h = ch_h_px + ch_h_px * (n_ch - 1) * (1.0f - overlap);
        int   vp_h    = model_->view()->getViewportHeight();

        if (scene_h > 0.0f)
        {
            top_y  = static_cast<int>(current_visible_y_ / scene_h * h);
            rect_h = static_cast<int>(vp_h               / scene_h * h);
        }
        top_y  = qBound(0, top_y,  h);
        rect_h = qBound(1, rect_h, h - top_y);
        rect_h = qMax(6, rect_h);   // always large enough to grab
        if (top_y + rect_h > h)
            top_y = qMax(0, h - rect_h);
    }

    return QRect(left_x, top_y, rect_w, rect_h);
}

//-----------------------------------------------------------------------------
int OverviewWidget::pixelToSceneY(int py) const
{
    auto shown = model_->getShownChannels();
    int  n_ch  = static_cast<int>(shown.size());
    if (n_ch == 0)
        return 0;
    float ch_h_px = static_cast<float>(
        model_->getSignalViewSettings()->getChannelHeight());
    float overlap = model_->getSignalViewSettings()->getChannelOverlapping();
    float scene_h = ch_h_px + ch_h_px * (n_ch - 1) * (1.0f - overlap);
    int h = height();
    if (h <= 0 || scene_h <= 0.0f)
        return 0;
    return static_cast<int>(static_cast<double>(py) / h * scene_h);
}

//-----------------------------------------------------------------------------
unsigned OverviewWidget::pixelToSample(int x) const
{
    const ChannelManager& cm         = model_->getChannelManager();
    unsigned              total_samp = static_cast<unsigned>(cm.getNumberSamples());
    int w                            = width();
    if (w <= 0 || total_samp == 0)
        return 0;
    return static_cast<unsigned>(static_cast<double>(x) / w * total_samp);
}

//-----------------------------------------------------------------------------
void OverviewWidget::mousePressEvent(QMouseEvent* event)
{
    if (!model_ || event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    if (event->pos().y() < RESIZE_HANDLE_HEIGHT)
    {
        resize_dragging_       = true;
        resize_start_global_y_ = event->globalPosition().toPoint().y();
        resize_start_height_   = height();
        setCursor(Qt::SizeVerCursor);
        return;
    }

    const QRect vr = computeViewportRect();
    const int   x  = event->pos().x();

    if (vr.isValid() && vr.contains(event->pos()))
    {
        dragging_       = true;
        drag_offset_px_ = x - vr.left();
        drag_offset_py_ = event->pos().y() - vr.top();
        setCursor(Qt::SizeAllCursor);
    }
    else
    {
        const ChannelManager& cm         = model_->getChannelManager();
        const unsigned        total_samp = static_cast<unsigned>(cm.getNumberSamples());
        if (total_samp == 0)
            return;

        const int      vp_px = model_->view() ? model_->view()->getViewportWidth() : 0;
        const float    pps   = model_->getSignalViewSettings()->getPixelsPerSample();
        const unsigned half  = (pps > 0.0f)
                               ? static_cast<unsigned>(vp_px / pps / 2)
                               : 0u;
        unsigned target = pixelToSample(x);
        target = (target >= half) ? target - half : 0u;
        model_->goToSample(target);
        update();
    }
}

//-----------------------------------------------------------------------------
void OverviewWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (resize_dragging_)
    {
        const int delta = resize_start_global_y_ - event->globalPosition().toPoint().y();
        const int new_h = qBound(MIN_HEIGHT, resize_start_height_ + delta, MAX_HEIGHT);
        if (new_h != height())
            setFixedHeight(new_h);
        return;
    }

    if (!dragging_ || !model_)
    {
        QRect vr = computeViewportRect();
        if (event->pos().y() < RESIZE_HANDLE_HEIGHT)
            setCursor(Qt::SizeVerCursor);
        else
            setCursor((vr.isValid() && vr.contains(event->pos()))
                      ? Qt::SizeAllCursor : Qt::ArrowCursor);
        QWidget::mouseMoveEvent(event);
        return;
    }

    int x = event->pos().x() - drag_offset_px_;
    model_->goToSample(pixelToSample(qMax(0, x)));
    int oy = qBound(0, event->pos().y() - drag_offset_py_, height() - 1);
    emit requestScrollY(pixelToSceneY(oy));
    update();
}

//-----------------------------------------------------------------------------
void OverviewWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && resize_dragging_)
    {
        resize_dragging_ = false;
        QSettings().setValue("MainWindow/overviewHeight", height());
        unsetCursor();
        return;
    }
    if (event->button() == Qt::LeftButton && dragging_)
    {
        dragging_ = false;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
void OverviewWidget::leaveEvent(QEvent* event)
{
    if (!dragging_ && !resize_dragging_)
        unsetCursor();
    QWidget::leaveEvent(event);
}

} // namespace sigviewer
