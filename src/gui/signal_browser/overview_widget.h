// © SigViewer developers
//
// License: GPL-3.0

#ifndef OVERVIEW_WIDGET_H
#define OVERVIEW_WIDGET_H

#include "base/sigviewer_user_types.h"
#include "gui/signal_visualisation_model.h"
#include "gui/color_manager.h"
#include "file_handling/event_manager.h"

#include <QPixmap>
#include <QSharedPointer>
#include <QWidget>

namespace sigviewer
{

/// Shows a static minimap of all (detrended) channels over the full time
/// range below the main signal browser.  The currently visible portion is
/// highlighted and can be dragged to pan the view.
class OverviewWidget : public QWidget
{
    Q_OBJECT
public:
    OverviewWidget(QSharedPointer<SignalVisualisationModel> model,
                   QSharedPointer<ColorManager const> color_manager,
                   QSharedPointer<EventManager const> event_manager,
                   QWidget* parent = nullptr);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    /// Viewport rect moved, just repaint.
    void updateOverview();

    /// Vertical scroll position changed.
    void setVisibleY(int32 y);

    /// Signal data or channel set changed — re-render the pixmap.
    void rebuildSignalImage();

signals:
    void requestScrollY(int scene_y);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void     renderSignals();
    QRect    computeViewportRect() const;
    unsigned pixelToSample(int x) const;
    int      pixelToSceneY(int py) const;

    QSharedPointer<SignalVisualisationModel> model_;
    QSharedPointer<ColorManager const>       color_manager_;
    QSharedPointer<EventManager const>       event_manager_;

    QPixmap signal_pixmap_;
    bool    pixmap_dirty_       = true;

    bool    dragging_           = false;
    int     drag_offset_px_     = 0;
    int     drag_offset_py_     = 0;
    int32   current_visible_y_  = 0;

    bool    resize_dragging_        = false;
    int     resize_start_global_y_  = 0;
    int     resize_start_height_    = 0;
};

} // namespace sigviewer

#endif // OVERVIEW_WIDGET_H
