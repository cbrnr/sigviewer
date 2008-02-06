// smart_canvas_rectangle.h

#ifndef SMART_CANVAS_RECTANGLE_H
#define SMART_CANVAS_RECTANGLE_H

#include "smart_canvas.h"

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

class Q3PopupMenu;
class Q3CanvasItemList;

namespace BioSig_
{

class SmartCanvasView;

// smart canvas rectangle
class SmartCanvasRectangle : public Q3CanvasRectangle
{
public:
    enum
    {
        RTTI = SmartCanvas::MAX_CHUNK_MATRIX_SIZE + 0x000
    };

    SmartCanvasRectangle(SmartCanvas* canvas);
    SmartCanvasRectangle(const QRect& rect, SmartCanvas* canvas);
    SmartCanvasRectangle(int32 x, int32 y, int32 width, int32 height,
                         SmartCanvas* canvas) ;

    virtual int rtti() const;

    virtual void mousePressEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                 int32 first_hit, SmartCanvasView* canvas_view);
    virtual void mouseReleaseEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                   int32 first_hit,
                                   SmartCanvasView* canvas_view);
    virtual void mouseDoubleClickEvent(QMouseEvent* e,
                                       Q3CanvasItemList& item_list,
                                       int32 first_hit,
                                       SmartCanvasView* canvas_view);
    virtual void wheelEvent(QWheelEvent* e, Q3CanvasItemList& item_list,
                            int32 first_hit, SmartCanvasView* canvas_view);
    virtual void mouseMoveEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                SmartCanvasView* canvas_view);
    virtual void keyPressEvent(QKeyEvent* e, Q3CanvasItemList& item_list,
                               SmartCanvasView* canvas_view);
    virtual void keyReleaseEvent(QKeyEvent* e, Q3CanvasItemList& item_list,
                                 SmartCanvasView* canvas_view);
    virtual void contextMenuEvent(QContextMenuEvent* e,
                                  Q3CanvasItemList& item_list, int32 first_hit,
                                  Q3PopupMenu* menu,
                                  SmartCanvasView* canvas_view);
    virtual void toolTipText(const QPoint& p, Q3CanvasItemList& item_list,
                             QString* tool_tip);

protected:
    virtual void drawShape(QPainter& painter);
    virtual void smartDraw(QPainter& painter, const QRect& clip) = 0;
};

} // namespace BioSig_

#endif
