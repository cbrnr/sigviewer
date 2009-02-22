// smart_canvas_view.cpp

#include "smart_canvas_view.h"
#include "smart_canvas.h"
#include "smart_canvas_rectangle.h"

#include <q3popupmenu.h>

#include <QMap>
#include <QPainter>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QToolTip>

namespace BioSig_
{

// constructor
SmartCanvasView::SmartCanvasView(SmartCanvas* canvas, QWidget* parent,
                                 const char* name, Qt::WFlags f)
: Q3CanvasView(canvas, parent, name, f),
  tooltip_enabled_(true)
{
    // nothing
}

// destructor
SmartCanvasView::~SmartCanvasView()
{
    // nothing
}

// add event listener
void SmartCanvasView::addEventListener(int32 smart_events, Q3CanvasItem* item)
{
    if (smart_events & MOUSE_PRESS_EVENT)
    {
        if (press_items_.find(item) == press_items_.end())
        {
            press_items_.push_back(item);
        }
    }
    if (smart_events & MOUSE_RELEASE_EVENT)
    {
        if (release_items_.find(item) == release_items_.end())
        {
            release_items_.push_back(item);
        }
    }
    if (smart_events & MOUSE_DOUBLE_CLICK_EVENT)
    {
        if (double_click_items_.find(item) == double_click_items_.end())
        {
            double_click_items_.push_back(item);
        }
    }
    if (smart_events & MOUSE_MOVE_EVENT)
    {
        if (move_items_.find(item) == move_items_.end())
        {
            move_items_.push_back(item);
        }
    }
    if (smart_events & WHEEL_EVENT)
    {
        if (wheel_items_.find(item) == wheel_items_.end())
        {
            wheel_items_.push_back(item);
        }
    }
    if (smart_events & KEY_PRESS_EVENT)
    {
        if (key_press_items_.find(item) == key_press_items_.end())
        {
            key_press_items_.push_back(item);
        }
    }
    if (smart_events & KEY_RELEASE_EVENT)
    {
        if (key_release_items_.find(item) == key_release_items_.end())
        {
            key_release_items_.push_back(item);
        }
    }
    if (smart_events & CONTEXT_MENU_EVENT)
    {
        if (context_menu_items_.find(item) == context_menu_items_.end())
        {
            context_menu_items_.push_back(item);
        }
    }
}

// remove event listener
void SmartCanvasView::removeEventListener(int32 smart_events,
                                          Q3CanvasItem* item)
{
    if (smart_events & MOUSE_PRESS_EVENT)
    {
        press_items_.remove(item);
    }
    if (smart_events & MOUSE_RELEASE_EVENT)
    {
        release_items_.remove(item);
    }
    if (smart_events & MOUSE_DOUBLE_CLICK_EVENT)
    {
        double_click_items_.remove(item);
    }
    if (smart_events & MOUSE_MOVE_EVENT)
    {
        move_items_.remove(item);
    }
    if (smart_events & WHEEL_EVENT)
    {
        wheel_items_.remove(item);
    }
    if (smart_events & KEY_PRESS_EVENT)
    {
        key_press_items_.remove(item);
    }
    if (smart_events & KEY_RELEASE_EVENT)
    {
        key_release_items_.remove(item);
    }
    if (smart_events & CONTEXT_MENU_EVENT)
    {
        context_menu_items_.remove(item);
    }
}

// contents mouse press event
void SmartCanvasView::contentsMousePressEvent(QMouseEvent* e)
{
    Q3CanvasItemList tmp_list(press_items_);
    int32 first_hit = tmp_list.size();
    tmp_list += canvas()->collisions(inverseWorldMatrix().map(e->pos()));
    for (Q3CanvasItemList::Iterator it = tmp_list.begin();
         it != tmp_list.end();
         it++)
    {
        if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
        {
            ((SmartCanvasRectangle*)*it)->mousePressEvent(e, tmp_list,
                                                          first_hit, this);
        }
    }
}

// contents mouse release event
void SmartCanvasView::contentsMouseReleaseEvent(QMouseEvent* e)
{
    Q3CanvasItemList tmp_list(release_items_);
    int32 first_hit = tmp_list.size();
    tmp_list += canvas()->collisions(inverseWorldMatrix().map(e->pos()));
    for (Q3CanvasItemList::Iterator it = tmp_list.begin();
         it != tmp_list.end();
         it++)
    {
        if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
        {
            ((SmartCanvasRectangle*)*it)->mouseReleaseEvent(e, tmp_list,
                                                            first_hit, this);
        }
    }
}

// contents mouse doubel click event
void SmartCanvasView::contentsMouseDoubleClickEvent(QMouseEvent* e)
{
    Q3CanvasItemList tmp_list(double_click_items_);
    int32 first_hit = tmp_list.size();
    tmp_list += canvas()->collisions(inverseWorldMatrix().map(e->pos()));
    for (Q3CanvasItemList::Iterator it = tmp_list.begin();
         it != tmp_list.end();
         it++)
    {
        if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
        {
            ((SmartCanvasRectangle*)*it)->mouseDoubleClickEvent(e, tmp_list,
                                                                first_hit,
                                                                this);
        }
    }
}

// contents wheel event
void SmartCanvasView::contentsWheelEvent(QWheelEvent* e)
{
    Q3CanvasItemList tmp_list(wheel_items_);
    int32 first_hit = tmp_list.size();
    tmp_list += canvas()->collisions(inverseWorldMatrix().map(e->pos()));
    for (Q3CanvasItemList::Iterator it = tmp_list.begin();
         it != tmp_list.end();
         it++)
    {
        if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
        {
            ((SmartCanvasRectangle*)*it)->wheelEvent(e, tmp_list, first_hit,
                                                     this);
        }
    }
}

// contents mouse move event
void SmartCanvasView::contentsMouseMoveEvent(QMouseEvent* e)
{
    Q3CanvasItemList tmp_list(move_items_);
    for (Q3CanvasItemList::Iterator it = tmp_list.begin();
         it != tmp_list.end();
         it++)
    {
        if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
        {
            ((SmartCanvasRectangle*)*it)->mouseMoveEvent(e, tmp_list, this);
        }
    }
}

// key pressed event
void SmartCanvasView::keyPressEvent(QKeyEvent* e)
{
    Q3CanvasItemList tmp_list(key_press_items_);
    for (Q3CanvasItemList::Iterator it = tmp_list.begin();
         it != tmp_list.end();
         it++)
    {
        if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
        {
            ((SmartCanvasRectangle*)*it)->keyPressEvent(e, tmp_list, this);
        }
    }
}

// key release event
void SmartCanvasView::keyReleaseEvent(QKeyEvent* e)
{
    Q3CanvasItemList tmp_list(key_release_items_);
    for (Q3CanvasItemList::Iterator it = tmp_list.begin();
         it != tmp_list.end();
         it++)
    {
        if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
        {
            ((SmartCanvasRectangle*)*it)->keyReleaseEvent(e, tmp_list, this);
        }
    }
}

// context menu event
void SmartCanvasView::contextMenuEvent(QContextMenuEvent* e)
{
    Q3PopupMenu* context_menu = new Q3PopupMenu(this);
    Q3CanvasItemList tmp_list(context_menu_items_);
    int32 first_hit = tmp_list.size();
    QPoint p = inverseWorldMatrix().map(viewportToContents(e->pos()));
    tmp_list += canvas()->collisions(inverseWorldMatrix().map(p));
    for (Q3CanvasItemList::Iterator it = tmp_list.begin();
         it != tmp_list.end();
         it++)
    {
        if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
        {
            ((SmartCanvasRectangle*)*it)->contextMenuEvent(e, tmp_list,
                                                           first_hit,
                                                           context_menu, this);
        }
    }

    if (context_menu->count() > 0)
    {
        context_menu->exec(e->globalPos());
    }
    delete context_menu;
}

// set tool tips enabled
void SmartCanvasView::setToolTipsEnabled(bool enabled)
{
    tooltip_enabled_ = enabled;
}

// event
bool SmartCanvasView::event(QEvent *e)
{
    if (e->type() != QEvent::ToolTip)
    {
        return Q3CanvasView::event(e);   
    }
    
    QHelpEvent* help_event = (QHelpEvent*)e;
    if (tooltip_enabled_)
    {
        QString text;
        Q3CanvasItemList tmp_list(context_menu_items_);
        QPoint p = inverseWorldMatrix().map(
                                        viewportToContents(help_event->pos()));
        tmp_list += canvas()->collisions(inverseWorldMatrix().map(p));
        for (Q3CanvasItemList::Iterator it = tmp_list.begin();
             it != tmp_list.end();
             it++)
        {
            if ((*it)->rtti() >= SmartCanvas::SMART_CANVAS_ITEM_RTTI_OFFSET)
            {
                ((SmartCanvasRectangle*)*it)->toolTipText(p, tmp_list, &text);
            }
        }
        QToolTip::showText(help_event->globalPos(), text);
    }
    return true;
}

} // namespace BioSig_

