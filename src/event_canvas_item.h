// event_canvas_item.h

#ifndef EVENT_CANVAS_ITEM_H
#define EVENT_CANVAS_ITEM_H

#include "smart_canvas/smart_canvas_rectangle.h"
#include "base/signal_buffer.h"
#include "signal_browser_model.h"

#include <QMap>
#include <QObject>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <Q3PopupMenu>

class QMouseEvent;
class QImage;

namespace BioSig_
{

class SmartCanvasView;
class SignalBrowser;

// event canvas item
class EventCanvasItem : private QObject,
                        public SmartCanvasRectangle
{
    Q_OBJECT
public:
    enum
    {
        RTTI = SmartCanvasRectangle::RTTI + 0x3
    };

    enum Action
    {
        ACTION_NONE,
        ACTION_MOVE_BEGIN,
        ACTION_MOVE_END,
        ACTION_SHIFT_TO_CHANNEL,
        ACTION_COPY_SHIFT_TO_CHANNEL,
        ACTION_SELECT
    };

    EventCanvasItem(uint32 id, SignalBuffer& buffer, SignalBrowserModel& model,
                    SignalBrowser* browser);
    ~EventCanvasItem();

    uint32 getId() const;

    virtual int rtti() const;

    const QColor& getColor() const;
    void updateColor();

    static void loadSettings();
    static void saveSettings();

    Action getMousePressAction(QMouseEvent* e, SmartCanvasView* canvas_view);
    void startMouseMoveEnd(SmartCanvasView* canvas_view);

    virtual void mousePressEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                 int32 first_hit, SmartCanvasView* canvas_view);
    virtual void mouseReleaseEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                   int32 first_hit,
                                   SmartCanvasView* canvas_view);
    virtual void mouseMoveEvent(QMouseEvent* e, Q3CanvasItemList& item_list,
                                SmartCanvasView* canvas_view);
    virtual void contextMenuEvent(QContextMenuEvent* e,
                                  Q3CanvasItemList& item_list, int32 first_hit,
                                  Q3PopupMenu* menu,
                                  SmartCanvasView* canvas_view);
    virtual void toolTipText(const QPoint& p, Q3CanvasItemList& item_list,
                             QString* tool_tip);

protected:
    void smartDraw(QPainter& p, const QRect& clip);

private slots:
    void select();

private:
    typedef QMap<int32, QString> Int2StringMap;

    enum State
    {
        STATE_NONE,
        STATE_MOVE_BEGIN,
        STATE_MOVE_END,
        STATE_SHIFT_TO_CHANNEL
    };

    SignalBuffer& signal_buffer_;
    SignalBrowserModel& signal_browser_model_;
    SignalBrowser* signal_browser_;
    uint32 id_;
    QColor color_;
    State state_;
    SmartCanvasView* canvas_view_;
    static int move_mouse_range_;
    int32 last_shift_shown_nr_;
};

} // namespace BioSig_

#endif
