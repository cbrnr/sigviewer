// smart_canvas_rectangle.cpp

#include "smart_canvas_rectangle.h"

#include <Q3PopupMenu>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

namespace BioSig_
{

// constructor
SmartCanvasRectangle::SmartCanvasRectangle(SmartCanvas* canvas)
: Q3CanvasRectangle(canvas)
{
    // nothing
}

// constructor
SmartCanvasRectangle::SmartCanvasRectangle(const QRect& rect,
                                           SmartCanvas* canvas)
: Q3CanvasRectangle(rect, canvas)
{
    // nothing
}

// constructor
SmartCanvasRectangle::SmartCanvasRectangle(int32 x, int32 y, int32 width,
                                           int32 height, SmartCanvas* canvas)
: Q3CanvasRectangle(x, y , width, height, canvas)
{
    // nothing
}

// rtti
int SmartCanvasRectangle::rtti() const
{
    return RTTI;
}

// draw shape
void SmartCanvasRectangle::drawShape(QPainter& painter)
{
    smartDraw(painter, ((SmartCanvas*)canvas())->getLastDrawBackgroundClip());
}

// mouse press event
void SmartCanvasRectangle::mousePressEvent(QMouseEvent*, Q3CanvasItemList&,
                                           int32, SmartCanvasView*)
{
    // nothing
}

// mouse release event
void SmartCanvasRectangle::mouseReleaseEvent(QMouseEvent*, Q3CanvasItemList&,
                                             int32, SmartCanvasView*)
{
    // nothing
}

// mouse double click event
void SmartCanvasRectangle::mouseDoubleClickEvent(QMouseEvent*, Q3CanvasItemList&,
                                                 int32, SmartCanvasView*)
{
    // nothing
}

// wheel event
void SmartCanvasRectangle::wheelEvent(QWheelEvent*, Q3CanvasItemList&, int32,
                                      SmartCanvasView*)
{
    // nothing
}

// mouse move event
void SmartCanvasRectangle::mouseMoveEvent(QMouseEvent*, Q3CanvasItemList&,
                                          SmartCanvasView*)
{
    // nothing
}

// key press event
void SmartCanvasRectangle::keyPressEvent(QKeyEvent*, Q3CanvasItemList&,
                                         SmartCanvasView*)
{
    // nothing
}

// key release event
void SmartCanvasRectangle::keyReleaseEvent(QKeyEvent*, Q3CanvasItemList&,
                                           SmartCanvasView*)
{
    // nothing
}

// context menu event   
void SmartCanvasRectangle::contextMenuEvent(QContextMenuEvent*,
                                            Q3CanvasItemList&, int32,
                                            Q3PopupMenu*, SmartCanvasView*)
{
    // nothing
}

// tool tip text
void SmartCanvasRectangle::toolTipText(const QPoint&, Q3CanvasItemList&,
                                       QString*)
{
    // nothing
}

} // namespace BioSig_
