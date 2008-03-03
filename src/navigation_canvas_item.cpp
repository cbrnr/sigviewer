// navigation_canvas_item.cpp

#include "navigation_canvas_item.h"
#include "smart_canvas/smart_canvas_view.h"
#include "signal_browser.h"
#include "main_window.h"
#include "main_window_model.h"
#include "signal_browser_mouse_handling.h"

#include <q3popupmenu.h>

#include <QEvent>
#include <QCursor>
#include <QPainter>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>

namespace BioSig_
{

// constructor
NavigationCanvasItem::NavigationCanvasItem(SignalBrowserModel& model,
                                 SignalBrowser* browser)
: SmartCanvasRectangle(browser->getCanvas()),
  signal_browser_model_(model),
  signal_browser_(browser),
  navigation_(NO_NAVIGATION),
  zoom_top_line_(2000, 1),
  zoom_bottom_line_(2000, 1),
  zoom_left_line_(1, 2000),
  zoom_right_line_(1, 2000)
{
    // nothing
}

// rtti
int NavigationCanvasItem::rtti() const
{
    return RTTI;
}

// smart draw
void NavigationCanvasItem::smartDraw(QPainter&, const QRect&)
{
    // nothing
}

// mouse press event
void NavigationCanvasItem::mousePressEvent(QMouseEvent* e, Q3CanvasItemList&,
                                           int32, SmartCanvasView* canvas_view)
{
    if (navigation_ != NO_NAVIGATION)
    {
        return;
    }  
    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    SignalBrowserMouseHandling::Action action = SignalBrowserMouseHandling::getAction(e, mode);
    switch (action)
    {
        case SignalBrowserMouseHandling::HAND_SCROLL_ACTION :
            signal_browser_->getCanvasView()->viewport()
                ->setCursor(QCursor(Qt::SizeAllCursor));
            navigation_ = SCROLL_NAVIGATION;
            first_point_ = e->globalPos();
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            break;
        case SignalBrowserMouseHandling::NEW_EVENT_ACTION :
            navigation_ = NEW_EVENT_NAVIGATION;
            first_point_ = e->globalPos();
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            break;
        case SignalBrowserMouseHandling::ZOOM_WINDOW_ACTION :
            navigation_ = ZOOM_NAVIGATION;
            canvas_view->addEventListener(SmartCanvasView::MOUSE_RELEASE_EVENT |
                                          SmartCanvasView::MOUSE_MOVE_EVENT,
                                          this);
            first_point_ = signal_browser_->getCanvasView()
                                        ->inverseWorldMatrix().map(e->pos());
            second_point_ = first_point_;
            drawZoomRect();
            break;
        default:
            break;    
    }
}

// mouse release event
void NavigationCanvasItem::mouseReleaseEvent(QMouseEvent*, Q3CanvasItemList&,
                                             int32,
                                             SmartCanvasView* canvas_view)
{
    switch(navigation_)
    {
        case NO_NAVIGATION:
            break;
        case SCROLL_NAVIGATION:
            navigation_ = NO_NAVIGATION;
            signal_browser_->getCanvasView()->viewport()
                                    ->setCursor(QCursor(Qt::ArrowCursor));
            canvas_view->removeEventListener(
                                        SmartCanvasView::MOUSE_RELEASE_EVENT |
                                        SmartCanvasView::MOUSE_MOVE_EVENT,
                                        this);
            break;
        case NEW_EVENT_NAVIGATION:
            navigation_ = NO_NAVIGATION;
            canvas_view->removeEventListener(
                                        SmartCanvasView::MOUSE_RELEASE_EVENT |
                                        SmartCanvasView::MOUSE_MOVE_EVENT,
                                        this);
            break;
        case ZOOM_NAVIGATION:
            navigation_ = NO_NAVIGATION;
            canvas_view->removeEventListener(
                                        SmartCanvasView::MOUSE_RELEASE_EVENT |
                                        SmartCanvasView::MOUSE_MOVE_EVENT,
                                        this);
            clearZoomRect();
            QRect rect(first_point_, second_point_);
            signal_browser_model_.zoomRect(rect.normalize());
            break;

    }
}

// mouse move event
void NavigationCanvasItem::mouseMoveEvent(QMouseEvent* e, Q3CanvasItemList&,
                                          SmartCanvasView* canvas_view)
{  
    switch(navigation_)
    {
        case NO_NAVIGATION:
            break;
        case SCROLL_NAVIGATION:
            {
                second_point_ = e->globalPos();
                int32 dx = second_point_.x() - first_point_.x();
                int32 dy = second_point_.y() - first_point_.y();
                first_point_ = second_point_;
                signal_browser_->getCanvasView()->scrollBy(-dx, -dy);
            }
            break;
        case NEW_EVENT_NAVIGATION:
            {
                int32 const EDGE_OFFSET = 50;
                int32 w = signal_browser_->getCanvasView()->visibleWidth ();
                int32 offset = signal_browser_->getCanvasView()->window()->pos().x() + EDGE_OFFSET;
                
                second_point_ = e->globalPos();
                int32 dx = second_point_.x() - first_point_.x();
                first_point_ = second_point_;
                if ((e->globalX() - offset > w && dx > 0) || (e->globalX() - offset < EDGE_OFFSET && dx < 0))
                    signal_browser_->getCanvasView()->scrollBy(dx, 0);
            }
            break;
        case ZOOM_NAVIGATION:
            {
                clearZoomRect();
                int32 w = width();
                int32 h = height();
                second_point_ = canvas_view->inverseWorldMatrix().map(e->pos());
                int32 x = second_point_.x();
                int32 y = second_point_.y();
                second_point_.setX(x < 0 ? 0 : x > w -1 ? w - 1 : x);
                second_point_.setY(y < 0 ? 0 : y > h -1 ? h - 1 : y);
                drawZoomRect();
            }
            break;
    }
}

// wheel event
void NavigationCanvasItem::wheelEvent(QWheelEvent* e, Q3CanvasItemList&,
                                      int32, SmartCanvasView*)
{
    if (navigation_ != NO_NAVIGATION)
    {
        return;
    }
    SignalBrowserModel::Mode mode = signal_browser_model_.getMode();
    switch (SignalBrowserMouseHandling::getAction(e, mode))
    {
        case SignalBrowserMouseHandling::WHEEL_SCROLL_ACTION:
            if (e->delta() > 0)
            {
                int one_signal_step = signal_browser_model_.getSignalHeight() +
                                      signal_browser_model_.getSignalSpacing();
                signal_browser_->getCanvasView()->scrollBy(0, -one_signal_step);
            }
            else
            {
                int one_signal_step = signal_browser_model_.getSignalHeight() +
                                      signal_browser_model_.getSignalSpacing();
                signal_browser_->getCanvasView()->scrollBy(0, one_signal_step);
            }        
            break;
        case SignalBrowserMouseHandling::ZOOM_ALL_CHANNELS_ACTION :
            if (e->delta() > 0)
            {
                signal_browser_model_.zoomInAll();
            }
            else
            {
                signal_browser_model_.zoomOutAll();
            }              
        default:
            break;   
    }
}

// draw zoom rect
void NavigationCanvasItem::drawZoomRect()
{
    // TODO !!!!
    SmartCanvasView* canvas_view = signal_browser_->getCanvasView();
    QWidget* view_port = canvas_view->viewport();
    QRect rect(canvas_view->contentsToViewport(first_point_),
               canvas_view->contentsToViewport(second_point_));
    rect = rect.normalize();
    bitBlt(&zoom_top_line_, 0, 0,
           view_port, rect.left(), rect.top(), rect.width(), 1);
    bitBlt(&zoom_bottom_line_, 0, 0,
           view_port, rect.left(), rect.bottom(), rect.width(), 1);
    bitBlt(&zoom_left_line_, 0, 0,
           view_port, rect.left(), rect.top(), 1, rect.height());
    bitBlt(&zoom_right_line_, 0, 0,
           view_port, rect.right(), rect.top(), 1, rect.height());
    QPainter p(view_port);
    p.setPen(Qt::black);
    p.drawRect(rect);
}

// clear zoom rect
void NavigationCanvasItem::clearZoomRect()
{
    QWidget* view_port = signal_browser_->getCanvasView()->viewport();
    SmartCanvasView* canvas_view = signal_browser_->getCanvasView();
    QRect rect(canvas_view->contentsToViewport(first_point_),
               canvas_view->contentsToViewport(second_point_));
    rect = rect.normalize();
    bitBlt(view_port, rect.left(), rect.top(),
           &zoom_top_line_, 0, 0, rect.width(), 1);
    bitBlt(view_port, rect.left(), rect.bottom(),
           &zoom_bottom_line_, 0, 0, rect.width(), 1);
    bitBlt(view_port, rect.left(), rect.top(),
           &zoom_left_line_, 0, 0, 1, rect.height());
    bitBlt(view_port, rect.right(), rect.top(),
           &zoom_right_line_, 0, 0, 1, rect.height());
}

// context menu event
void NavigationCanvasItem::contextMenuEvent(QContextMenuEvent*,
                                            Q3CanvasItemList&, int32,
                                            Q3PopupMenu* menu,
                                            SmartCanvasView*)
{
    if (menu->count() > 0)
    {
        menu->insertSeparator();
    }
    MainWindow* main_window;
    main_window = signal_browser_model_.getMainWindowModel().getMainWindow();
    main_window->addActionTo(menu, "view_zoom_in_action_");
    main_window->addActionTo(menu, "view_zoom_out_action_");
    main_window->addActionTo(menu, "view_auto_scale_action_");
}

} // namespace BioSig_
