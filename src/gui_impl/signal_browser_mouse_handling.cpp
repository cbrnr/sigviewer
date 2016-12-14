#include "signal_browser_mouse_handling.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>

namespace SigViewer_
{

SignalBrowserMouseHandling::Action SignalBrowserMouseHandling::getAction(QGraphicsSceneMouseEvent* e, SignalVisualisationMode mode)
{
        // hand scroll
    if ((e->button() == Qt::MidButton  && e->modifiers() == Qt::NoModifier) ||
        (mode == MODE_HAND && e->button() == Qt::LeftButton))
    {
        return HAND_SCROLL_ACTION;
    }

    // shift channel
    if ((e->button() == Qt::MidButton && e->modifiers().testFlag(Qt::ShiftModifier)) ||
        (mode == MODE_VIEW_OPTIONS && e->button() == Qt::LeftButton))
    {
        return SHIFT_CHANNEL_ACTION;
    }

    // new event
    if (e->button() == Qt::LeftButton && mode == MODE_NEW)
    {
        return NEW_EVENT_ACTION;
    }

    // select or resize event
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::NoModifier &&
        mode == MODE_POINTER)
    {
        return SELECT_RESIZE_EVENT_ACTION;
    }

    // shift event to channel
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier &&
        mode == MODE_POINTER)
    {
        return SHIFT_EVENT_TO_CHANNEL_ACTION;
    }

    // copy event to channel
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier
        && mode == MODE_POINTER)
    {
        return COPY_EVENT_TO_CHANNEL_ACTION;
    }

    return NO_ACTION;
}

} // namespace SigViewer_
