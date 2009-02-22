// navigation_canvas_item.h

#ifndef NAVIGATION_CANVAS_ITEM_H
#define NAVIGATION_CANVAS_ITEM_H

#include "smart_canvas/smart_canvas_rectangle.h"
#include "signal_browser_model.h"

#include <QPixmap>

class QMouseEvent;
class QWheelEvent;
class Q3PopupMenu;
class QWheelEvent;
class QMouseEvent;
class QContextMenuEvent;

namespace BioSig_
{

class SmartCanvasView;
class SignalBrowser;

// x grid canvas item
class NavigationCanvasItem : public SmartCanvasRectangle
{
public:
    enum
    {
        RTTI = SmartCanvasRectangle::RTTI + 0x4
    };

    NavigationCanvasItem(SignalBrowserModel& model, SignalBrowser* browser);

    virtual int rtti() const;

    virtual void mousePressEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                 int32 first_hit, SmartCanvasView* canvas_view);
    virtual void mouseReleaseEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                   int32 first_hit,
                                   SmartCanvasView* canvas_view);
    virtual void wheelEvent(QWheelEvent* e, Q3CanvasItemList& item_list,
                            int32 first_hit, SmartCanvasView* canvas_view);
    virtual void mouseMoveEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                SmartCanvasView* canvas_view);
    virtual void contextMenuEvent(QContextMenuEvent* e,
                                  Q3CanvasItemList& item_list, int32 first_hit,
                                  Q3PopupMenu* menu,
                                  SmartCanvasView* canvas_view);

protected:
    void smartDraw(QPainter& p, const QRect& clip);

private:
    enum Navigation
    {
        NO_NAVIGATION = 0,
        SCROLL_NAVIGATION = 1,
        ZOOM_NAVIGATION = 2,
        NEW_EVENT_NAVIGATION = 3
    };

    void drawZoomRect();
    void clearZoomRect();

    SignalBrowserModel& signal_browser_model_;
    SignalBrowser* signal_browser_;
    Navigation navigation_;
    QPoint first_point_;
    QPoint second_point_;
    QPixmap zoom_top_line_;
    QPixmap zoom_bottom_line_;
    QPixmap zoom_left_line_;
    QPixmap zoom_right_line_;
};

} // namespace BioSig_

#endif
