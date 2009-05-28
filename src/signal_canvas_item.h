#ifndef QT4_PORTED
// signal_canvas_item.h

#ifndef SIGNAL_CANVAS_ITEM_H
#define SIGNAL_CANVAS_ITEM_H

#include "smart_canvas/smart_canvas_rectangle.h"
#include "base/signal_buffer.h"
#include "base/signal_channel.h"
#include "signal_browser_model.h"
#include "settings_dialog.h"

#include <QObject>

class QMouseEvent;
class QContextMenuEvent;
class QWheelEvent;

namespace BioSig_
{

class SmartCanvasView;
class SignalBrowser;

// signal canvas item
class SignalCanvasItem : private QObject,
                         public SmartCanvasRectangle
{
    Q_OBJECT

public:

    enum
    {
        RTTI = SmartCanvasRectangle::RTTI + 0x5
    };

    SignalCanvasItem(SignalBuffer& buffer, const SignalChannel& channel,
                     SignalBrowserModel& model, SignalBrowser* browser);

    virtual int rtti() const;

    static void loadSettings();
    static void saveSettings();

    void setYZoom(float64 zoom);
    float64 getYZoom();
    void setYOffset(float64 offset);
    float64 getYOffset();
    void getRangeFromBuffer(float64 factor);
    void updateYGridIntervall();
    float64 getMaximum();
    float64 getMinimum();
    float64 getYGridPixelIntervall();
    void enableYGrid(bool enabled);

    virtual void mousePressEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                 int32 first_hit, SmartCanvasView* canvas_view);

    virtual void mouseReleaseEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                   int32 first_hit,
                                   SmartCanvasView* canvas_view);

    virtual void mouseMoveEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                SmartCanvasView* canvas_view);

    virtual void wheelEvent(QWheelEvent* e, Q3CanvasItemList& item_list,
                            int32 first_hit, SmartCanvasView* canvas_view);

    virtual void contextMenuEvent(QContextMenuEvent* e,
                                  Q3CanvasItemList& item_list, int32 first_hit,
                                  Q3PopupMenu* menu,
                                  SmartCanvasView* canvas_view);

public slots:

    void zoomIn(bool update = true);
    void zoomOut(bool update = true);
    void autoScale(bool update = true, ScaleMode auto_zoom_type = MAX_TO_MAX);

protected:

    void smartDraw(QPainter& p, const QRect& clip);

private:

    SignalBuffer& signal_buffer_;
    const SignalChannel& signal_channel_;
    SignalBrowserModel& signal_browser_model_;
    SignalBrowser* signal_browser_;
    float64 y_zoom_;
    float64 y_offset_; // middle
    QPoint move_start_point_;
    bool shifting_;
    static float64 prefered_pixel_per_sample_;
    float64 minimum_;
    float64 maximum_;
    float64 y_grid_pixel_intervall_;
    bool drawYGrid_;
};

} // namespace BioSig_



#endif

#endif

