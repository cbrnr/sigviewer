// © SigViewer developers
//
// License: GPL-3.0

#ifndef OVERVIEW_WIDGET_H
#define OVERVIEW_WIDGET_H

#include "base/sigviewer_user_types.h"
#include "gui/signal_visualisation_model.h"
#include "gui/signal_view_settings.h"
#include "gui/color_manager.h"

#include <QWidget>
#include <QSharedPointer>
#include <QMap>
#include <QVector>
#include <QTimer>

#include <set>

namespace sigviewer
{

/// OverviewWidget
///
/// A minimap widget placed below the main signal browser that renders a
/// downsampled overview of all currently visible channels.  The currently
/// visible time/channel segment is highlighted with a semi-transparent rect.
/// The user can click anywhere to jump to that position, or drag the
/// highlight rect to pan the view.
///
/// Rendering is cache-based: the per-channel min/max overview is built once
/// in buildCache() and reused for every subsequent paintEvent.  The cache is
/// invalidated and rebuilt whenever the channel set, zoom, or underlying data
/// source changes.
class OverviewWidget : public QWidget
{
    Q_OBJECT
public:
    OverviewWidget(QSharedPointer<SignalVisualisationModel> model,
                   QSharedPointer<ColorManager const> color_manager,
                   QWidget* parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    /// Light repaint: only the viewport-highlight rect position changed.
    void updateOverview();

    /// Update Y scroll position and repaint.
    void setVisibleY(int32 y);

    /// Invalidate the pixel cache and schedule a rebuild.
    void scheduleCacheRebuild();

signals:
    /// Emitted during vertical drag; value is the target scene Y position.
    void requestScrollY(int scene_y);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void buildCache();

private:
    QRect    computeViewportRect() const;
    unsigned pixelToSample(int x) const;
    int      pixelToSceneY(int py) const;

    // Per-channel cached overview data (downsampled to cache_width_ pixels).
    struct ChannelOverview {
        QVector<float> min_vals;
        QVector<float> max_vals;
        float          norm_min = 0.0f;
        float          norm_max = 1.0f;
    };

    QSharedPointer<SignalVisualisationModel> model_;
    QSharedPointer<ColorManager const>      color_manager_;

    QMap<ChannelID, ChannelOverview> cache_;
    std::set<ChannelID>              cache_channel_set_;
    int                              cache_width_ = 0;
    bool                             cache_dirty_ = true;
    QTimer*                          rebuild_timer_;

    bool dragging_;
    int  drag_offset_px_;
    int  drag_offset_py_;
    int  current_visible_y_;
};

} // namespace sigviewer

#endif // OVERVIEW_WIDGET_H
