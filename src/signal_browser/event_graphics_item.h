#ifndef EVENT_GRAPHICS_ITEM_H
#define EVENT_GRAPHICS_ITEM_H


#include "../base/user_types.h"

#include <QGraphicsItem>
#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>
#include <QLabel>

class QMutex;

namespace BioSig_
{

class SignalBuffer;
class SignalEvent;
class SignalBrowserModel;
class EventContextMenu;


class EventGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    EventGraphicsItem(SignalBuffer& buffer, SignalBrowserModel& model,
                    QSharedPointer<SignalEvent> signal_event);

    virtual ~EventGraphicsItem ();

    int32 getId() const;

    //void setSize (int32 width, int32 height);
    void setSelected (bool selected);
    QSharedPointer<SignalEvent> getSignalEvent ();
    QSharedPointer<SignalEvent const> getSignalEvent () const;
    //void updateColor ();

    static bool displayContextMenu (QGraphicsSceneContextMenuEvent * event);
    static bool displaySelectionMenu (QGraphicsSceneMouseEvent* event);


    // have to be implemented otherwise
//    virtual void hoverMoveEvent (QGraphicsSceneHoverEvent * event);
//    virtual void hoverEnterEvent (QGraphicsSceneHoverEvent* event);
//    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent* event);

public slots:
    void updateToSignalEvent ();

signals:
    void mouseAtSecond (float64 sec);
    void mouseMoving (bool mouse_is_moving);
    void hoverEnterSignalEvent (QSharedPointer<SignalEvent const>);
    void hoverLeaveSignalEvent (QSharedPointer<SignalEvent const>);

private:
    virtual QRectF boundingRect () const;
    virtual void mouseMoveEvent (QGraphicsSceneMouseEvent * mouse_event);
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event);


    enum Action
    {
        ACTION_NONE,
        ACTION_MOVE_BEGIN,
        ACTION_MOVE_END,
        ACTION_SHIFT_TO_CHANNEL,
        ACTION_COPY_SHIFT_TO_CHANNEL,
        ACTION_SELECT
    };

    enum State
    {
        STATE_NONE,
        STATE_MOVE_BEGIN,
        STATE_MOVE_END,
        STATE_SHIFT_TO_CHANNEL
    };

    //-----------------------------------------------------------------------------
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void contextMenuEvent (QGraphicsSceneContextMenuEvent * event);

    //-----------------------------------------------------------------------------
    Action getMousePressAction(QGraphicsSceneMouseEvent* e);

    //-----------------------------------------------------------------------------
    void addContextMenuEntry ();

    SignalBrowserModel& signal_browser_model_;
    SignalBuffer& signal_buffer_;
    QColor color_;
    State state_;

    int32 height_;
    int32 width_;
    bool is_selected_;
    QSharedPointer<SignalEvent> signal_event_;

    static int move_mouse_range_;
    static QMutex event_handling_mutex_;

    static QMutex context_menu_mutex_;
    static QSharedPointer<EventContextMenu> context_menu_;

};

}
#endif // EVENT_GRAPHICS_ITEM_H
