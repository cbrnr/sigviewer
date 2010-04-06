#ifndef SIGNALGRAPHICSITEM_H
#define SIGNALGRAPHICSITEM_H

#include "../base/user_types.h"

#include <QGraphicsItem>
#include <QSharedPointer>
#include <QObject>

class QPoint;

namespace BioSig_
{

class SignalChannel;
class SignalEvent;
class SignalBrowserModel;
class EventGraphicsItem;
class EventManager;
class CommandExecuter;
class ChannelManager;

class SignalGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    SignalGraphicsItem(EventManager& event_manager,
                       CommandExecuter& command_executor,
                       ChannelManager& channel_manager,
                       ChannelID id,
                       const SignalChannel& channel,
                       SignalBrowserModel& model);

    virtual ~SignalGraphicsItem ();

    void setHeight (int32 height);
    virtual QRectF boundingRect () const;

    void updateYGridIntervall();
    void enableYGrid(bool enabled);
    void enableXGrid(bool enabled);
    QString const &getLabel () const;

    float64 getYZoom() const;
    float64 getYOffset() const;
    float64 getYGridPixelIntervall() const;

    void zoomIn();
    void zoomOut();
    void autoScale(ScaleMode auto_zoom_type = MAX_TO_MAX);

signals:
    void mouseAtSecond (float64 sec);
    void mouseMoving (bool mouse_is_moving);
    void shifting (int32 channel_nr);

private:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event);
    virtual void contextMenuEvent (QGraphicsSceneContextMenuEvent * event);
    virtual void wheelEvent (QGraphicsSceneWheelEvent * event);

    void drawYGrid (QPainter* painter, QStyleOptionGraphicsItem const* option);

    EventManager& event_manager_;
    CommandExecuter& command_executor_;
    ChannelManager& channel_manager_;
    ChannelID id_;
    const SignalChannel& signal_channel_;
    SignalBrowserModel& signal_browser_model_;

    float64 minimum_;
    float64 maximum_;

    float64 y_zoom_;
    float64 y_grid_pixel_intervall_;
    bool draw_y_grid_;
    bool draw_x_grid_;
    float64 y_offset_;
    int32 height_;
    uint32 width_;


    bool shifting_;
    bool new_event_;
    EventGraphicsItem* created_event_item_;
    bool hand_tool_on_;
    QPointF move_start_point_;

    QSharedPointer<SignalEvent> new_signal_event_;
    QColor new_event_color_;

    static float64 prefered_pixel_per_sample_;


};

}

#endif // SIGNALGRAPHICSITEM_H
