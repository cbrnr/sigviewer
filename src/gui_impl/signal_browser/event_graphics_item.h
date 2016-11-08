#ifndef EVENT_GRAPHICS_ITEM_H
#define EVENT_GRAPHICS_ITEM_H


#include "base/sigviewer_user_types.h"
#include "base/signal_event.h"
#include "command_executer.h"
#include "file_handling/event_manager.h"
#include "gui/color_manager.h"
#include "gui/signal_view_settings.h"

#include <QGraphicsObject>
#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>
#include <QLabel>

class QMutex;

namespace SigViewer_
{

class SignalBrowserModel;
class EventContextMenu;

class EventGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity WRITE setOpacity READ opacity)
#if QT_VERSION >= 0x040600
    Q_INTERFACES(QGraphicsItem)
#endif
public:
    EventGraphicsItem(SignalBrowserModel& model,
                      QSharedPointer<SignalViewSettings const> signal_view_settings,
                      QSharedPointer<SignalEvent const> signal_event,
                      QSharedPointer<EventManager> event_manager,
                      QSharedPointer<CommandExecuter> command_executer,
                      QSharedPointer<ColorManager const> color_manager);

    virtual ~EventGraphicsItem ();

    int32 getId() const;

    void setSelected (bool selected);
    QSharedPointer<SignalEvent const> getSignalEvent () const;

    static bool displayContextMenu (QGraphicsSceneContextMenuEvent* event,
                                    QMenu* channel_menu);
    static bool displaySelectionMenu (QGraphicsSceneMouseEvent* event);

public slots:
    void updateToSignalEvent ();

signals:
    void mouseAtSecond (float64 sec);
    void mouseMoving (bool mouse_is_moving);

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
    QSharedPointer<SignalViewSettings const> signal_view_settings_;
    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<CommandExecuter> command_executer_;
    QColor color_;
    State state_;

    int32 height_;
    int32 width_;
    bool is_selected_;
    QSharedPointer<SignalEvent const> signal_event_;
    QSharedPointer<ColorManager const> color_manager_;

    static int move_mouse_range_;
    static QMutex event_handling_mutex_;

    static QMutex context_menu_mutex_;
    static EventContextMenu* context_menu_;

};

}
#endif // EVENT_GRAPHICS_ITEM_H
