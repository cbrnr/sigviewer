// © SigViewer developers
//
// License: GPL-3.0

#include "overview_widget.h"
#include "base/data_block.h"

#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QResizeEvent>

#include <algorithm>
#include <cmath>
#include <limits>

namespace sigviewer
{

namespace
{
    constexpr int WIDGET_HEIGHT  = 80;
    constexpr int MIN_HEIGHT     = 30;
    /// Debounce delay for cache rebuilds (ms).
    constexpr int REBUILD_DELAY  = 80;
} // anonymous namespace

//-----------------------------------------------------------------------------
OverviewWidget::OverviewWidget(QSharedPointer<SignalVisualisationModel> model,
                               QSharedPointer<ColorManager const> color_manager,
                               QWidget* parent)
    : QWidget(parent),
      model_(model),
      color_manager_(color_manager),
      cache_dirty_(true),
      dragging_(false),
      drag_offset_px_(0),
      drag_offset_py_(0),
      current_visible_y_(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMouseTracking(true);

    rebuild_timer_ = new QTimer(this);
    rebuild_timer_->setSingleShot(true);
    rebuild_timer_->setInterval(REBUILD_DELAY);
    connect(rebuild_timer_, &QTimer::timeout, this, &OverviewWidget::buildCache);
}

//-----------------------------------------------------------------------------
QSize OverviewWidget::sizeHint() const
{
    return QSize(-1, WIDGET_HEIGHT);
}

//-----------------------------------------------------------------------------
QSize OverviewWidget::minimumSizeHint() const
{
    return QSize(0, MIN_HEIGHT);
}

//-----------------------------------------------------------------------------
void OverviewWidget::updateOverview()
{
    // Light update: only the viewport-rect position changed, use existing cache.
    update();
}

//-----------------------------------------------------------------------------
void OverviewWidget::setVisibleY(int32 y)
{
    current_visible_y_ = y;
    // Also schedule a cache rebuild: setVisibleY fires after model->update(),
    // which is called after detrend is toggled, so the cached data may be stale.
    scheduleCacheRebuild();
}

//-----------------------------------------------------------------------------
void OverviewWidget::scheduleCacheRebuild()
{
    cache_dirty_ = true;
    rebuild_timer_->start();   // restarts if already running
}

//-----------------------------------------------------------------------------
void OverviewWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // If the width changed the per-pixel bucket count is wrong — rebuild.
    if (event->size().width() != event->oldSize().width())
        scheduleCacheRebuild();
}

//-----------------------------------------------------------------------------
// Build the per-channel downsampled overview cache.
// Called once from the debounce timer; O(N) per channel but runs only when
// the channel set or data source changes, never inside paintEvent.
//-----------------------------------------------------------------------------
void OverviewWidget::buildCache()
{
    if (!model_)
        return;

    const int w = width();
    if (w <= 0)
        return;

    auto shown = model_->getShownChannels();

    // Always rebuild (cache_dirty_ was true when this slot was scheduled).
    const ChannelManager& cm         = model_->getChannelManager();
    const unsigned        total_samp = static_cast<unsigned>(cm.getNumberSamples());

    QMap<ChannelID, ChannelOverview> new_cache;

    if (total_samp > 0 && !shown.empty())
    {
        // Ideal downsampling factor: roughly 1 ds-sample per pixel.
        const unsigned ideal_ds = qMax(1u, total_samp / static_cast<unsigned>(w));

        for (ChannelID id : shown)
        {
            ChannelOverview entry;
            entry.min_vals.fill( std::numeric_limits<float>::max(), w);
            entry.max_vals.fill(-std::numeric_limits<float>::max(), w);

            // Per-channel amplitude range for normalisation.
            entry.norm_min = static_cast<float>(cm.getMinValue(id));
            entry.norm_max = static_cast<float>(cm.getMaxValue(id));
            if (entry.norm_min >= entry.norm_max)
            {
                entry.norm_min -= 1.0f;
                entry.norm_max += 1.0f;
            }

            // --- Fast path: use the pre-built min/max pyramid ---
            unsigned actual_ds = cm.getNearestDownsamplingFactor(id, ideal_ds);
            QSharedPointer<DataBlock const> ds_min;
            QSharedPointer<DataBlock const> ds_max;
            if (actual_ds > 1)
            {
                ds_min = cm.getDownsampledMin(id, actual_ds);
                ds_max = cm.getDownsampledMax(id, actual_ds);
            }

            if (ds_min && ds_max && ds_min->size() > 0)
            {
                const size_t ds_size = ds_min->size();
                for (int x = 0; x < w; ++x)
                {
                    unsigned s0 = static_cast<unsigned>(
                        static_cast<double>(x)     / w * total_samp);
                    unsigned s1 = static_cast<unsigned>(
                        static_cast<double>(x + 1) / w * total_samp);
                    if (s1 > total_samp) s1 = total_samp;
                    if (s0 >= s1) continue;

                    unsigned d0 = s0 / actual_ds;
                    unsigned d1 = (s1 - 1) / actual_ds;
                    if (d0 >= ds_size) continue;
                    if (d1 >= ds_size) d1 = static_cast<unsigned>(ds_size - 1);

                    float mn = (*ds_min)[d0], mx = (*ds_max)[d0];
                    for (unsigned i = d0 + 1; i <= d1; ++i)
                    {
                        const float lo = (*ds_min)[i];
                        const float hi = (*ds_max)[i];
                        if (lo < mn) mn = lo;
                        if (hi > mx) mx = hi;
                    }
                    entry.min_vals[x] = mn;
                    entry.max_vals[x] = mx;
                }
            }
            else
            {
                // --- Slow path: channel manager has no pyramid (e.g. detrend) ---
                // Read the full cached signal once and stride through it to fill
                // per-pixel min/max buckets.  Use a stride to bound the work:
                //   stride = max(1, N / (w*8))  → at least 8 samples per pixel
                // on average, which is enough for a visually accurate minimap.
                QSharedPointer<DataBlock const> full = cm.getData(id, 0, total_samp);
                if (!full || full->size() == 0)
                    continue;

                const size_t n    = full->size();
                const size_t step = std::max<size_t>(
                    1, n / (static_cast<size_t>(w) * 8));

                for (size_t s = 0; s < n; s += step)
                {
                    const float v = (*full)[s];
                    if (std::isnan(v)) continue;
                    int px = static_cast<int>(
                        static_cast<double>(s) / n * w);
                    if (px >= w) px = w - 1;
                    if (v < entry.min_vals[px]) entry.min_vals[px] = v;
                    if (v > entry.max_vals[px]) entry.max_vals[px] = v;
                }
            }

            // Robust normalization: use 5th/95th percentile of pixel bucket
            // extremes to suppress artifact spikes that would otherwise compress
            // the real signal into a horizontal line.
            QVector<float> p_mins, p_maxs;
            p_mins.reserve(w);
            p_maxs.reserve(w);
            for (int x = 0; x < w; ++x)
            {
                if (entry.min_vals[x] <= entry.max_vals[x])
                {
                    p_mins.append(entry.min_vals[x]);
                    p_maxs.append(entry.max_vals[x]);
                }
            }
            if (!p_mins.isEmpty())
            {
                std::sort(p_mins.begin(), p_mins.end());
                std::sort(p_maxs.begin(), p_maxs.end());
                const int n_p  = p_mins.size();
                const int lo   = qMax(0,     n_p * 5  / 100);
                const int hi   = qMin(n_p - 1, n_p * 95 / 100);
                const float norm_lo = p_mins[lo];
                const float norm_hi = p_maxs[hi];
                if (norm_lo < norm_hi)
                {
                    const float pad = (norm_hi - norm_lo) * 0.05f;
                    entry.norm_min = norm_lo - pad;
                    entry.norm_max = norm_hi + pad;
                }
                else
                {
                    // Flat signal (constant value) — show a unit range centred on it.
                    entry.norm_min = norm_lo - 1.0f;
                    entry.norm_max = norm_hi + 1.0f;
                }
            }
            else
            {
                entry.norm_min = -1.0f;
                entry.norm_max =  1.0f;
            }

            new_cache[id] = std::move(entry);
        }
    }

    cache_              = std::move(new_cache);
    cache_channel_set_  = shown;
    cache_width_        = w;
    cache_dirty_        = false;
    update();
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
void OverviewWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.fillRect(rect(), palette().base());

    // Top border separator.
    painter.setPen(QPen(palette().dark().color(), 1));
    painter.drawLine(0, 0, width() - 1, 0);

    if (!model_)
        return;

    // If the cache is invalid, just show the background and let the rebuild
    // timer paint fresh data shortly.
    if (cache_dirty_ || cache_.isEmpty())
    {
        if (cache_dirty_)
            scheduleCacheRebuild();
        // Draw the viewport rect even when the cache is being rebuilt.
        QRect vr = computeViewportRect();
        if (vr.isValid())
        {
            QColor fill = palette().highlight().color();
            fill.setAlpha(50);
            painter.fillRect(vr, fill);
            painter.setPen(QPen(palette().highlight().color(), 1));
            painter.drawRect(vr.adjusted(0, 1, -1, -1));
        }
        return;
    }

    const int w    = width();
    const int h    = height();
    if (w <= 0 || h <= 0)
        return;

    const int n_ch = static_cast<int>(cache_.size());

    int ch_idx = 0;
    for (auto it = cache_.begin(); it != cache_.end(); ++it, ++ch_idx)
    {
        const ChannelID        id    = it.key();
        const ChannelOverview& entry = it.value();

        // Use proper integer division so the last row always reaches h.
        const int y_top = (ch_idx       * h) / n_ch;
        const int y_bot = ((ch_idx + 1) * h) / n_ch;
        const int ch_h  = y_bot - y_top;
        if (ch_h <= 0)
            continue;

        const float range = entry.norm_max - entry.norm_min;
        if (range <= 0.0f)
            continue;

        QColor ch_color = color_manager_ ? color_manager_->getChannelColor(id)
                                         : palette().text().color();
        painter.setPen(ch_color);

        const int cols = qMin(w, entry.min_vals.size());
        for (int x = 0; x < cols; ++x)
        {
            const float mn = entry.min_vals[x];
            const float mx = entry.max_vals[x];
            if (mn > mx)   // no valid sample fell in this bucket
                continue;

            int y1 = y_bot - 1
                     - static_cast<int>((mx - entry.norm_min) / range * (ch_h - 1));
            int y2 = y_bot - 1
                     - static_cast<int>((mn - entry.norm_min) / range * (ch_h - 1));
            y1 = qBound(y_top, y1, y_bot - 1);
            y2 = qBound(y_top, y2, y_bot - 1);
            painter.drawLine(x, y1, x, y2);
        }
    }

    // Channel separator lines.
    painter.setPen(QPen(palette().dark().color(), 1));
    for (int i = 1; i < n_ch; ++i)
        painter.drawLine(0, (i * h) / n_ch, w - 1, (i * h) / n_ch);

    // Viewport highlight rectangle.
    QRect vr = computeViewportRect();
    if (vr.isValid())
    {
        QColor fill = palette().highlight().color();
        fill.setAlpha(50);
        painter.fillRect(vr, fill);
        painter.setPen(QPen(palette().highlight().color(), 1));
        painter.drawRect(vr.adjusted(0, 1, -1, -1));
    }
}

//-----------------------------------------------------------------------------
void OverviewWidget::mousePressEvent(QMouseEvent* event)
{
    if (!model_ || event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
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
    if (!dragging_ || !model_)
    {
        QRect vr = computeViewportRect();
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
    if (event->button() == Qt::LeftButton && dragging_)
    {
        dragging_ = false;
        setCursor(Qt::ArrowCursor);
    }
    QWidget::mouseReleaseEvent(event);
}

} // namespace sigviewer
