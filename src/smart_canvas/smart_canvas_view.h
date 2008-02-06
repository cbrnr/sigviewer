// smart_canvas_view.h

#ifndef SMART_CANVAS_VIEW_H
#define SMART_CANVAS_VIEW_H

#include "../base/user_types.h"

#include <q3canvas.h>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMoveEvent>
#include <QKeyEvent>

class QMouseEvent;
class QWheelEvent;
class QMoveEvent;
class QKeyEvent;
class QContextMenuEvent;

namespace BioSig_
{

class SmartCanvas;

// smart canvas view
class SmartCanvasView : public Q3CanvasView
{
public:
    enum
    {
        NO_HIT = -1
    };

    enum SmartEvents
    {
        MOUSE_PRESS_EVENT = 0x1,
        MOUSE_RELEASE_EVENT = 0x2,
        MOUSE_DOUBLE_CLICK_EVENT = 0x4,
        MOUSE_MOVE_EVENT = 0x8,
        WHEEL_EVENT = 0x10,
        KEY_PRESS_EVENT = 0x20,
        KEY_RELEASE_EVENT = 0x40,
        CONTEXT_MENU_EVENT = 0x80,
        ALL_EVENTS = MOUSE_PRESS_EVENT | MOUSE_RELEASE_EVENT |
                     MOUSE_DOUBLE_CLICK_EVENT | MOUSE_MOVE_EVENT | WHEEL_EVENT |
                     KEY_PRESS_EVENT | KEY_RELEASE_EVENT | CONTEXT_MENU_EVENT
    };

    SmartCanvasView(SmartCanvas* canvas, QWidget* parent = 0,
                    const char* name = 0, Qt::WFlags f = 0);
    ~SmartCanvasView();

    void addEventListener(int32 smart_events, Q3CanvasItem* item);
    void removeEventListener(int32 smart_events, Q3CanvasItem* item);

    void setToolTipsEnabled(bool enable);

    bool event(QEvent * e);

protected:
    virtual void contentsMousePressEvent(QMouseEvent* e);
    virtual void contentsMouseReleaseEvent(QMouseEvent* e);
    virtual void contentsMouseDoubleClickEvent(QMouseEvent* e);
    virtual void contentsMouseMoveEvent(QMouseEvent* e);
    virtual void contentsWheelEvent(QWheelEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void keyReleaseEvent(QKeyEvent* e);
    virtual void contextMenuEvent(QContextMenuEvent* e);

private:
    Q3CanvasItemList press_items_;
    Q3CanvasItemList release_items_;
    Q3CanvasItemList double_click_items_;
    Q3CanvasItemList move_items_;
    Q3CanvasItemList wheel_items_;
    Q3CanvasItemList key_press_items_;
    Q3CanvasItemList key_release_items_;
    Q3CanvasItemList context_menu_items_;
    bool tooltip_enabled_;
};

} // namespace BioSig_

#endif
