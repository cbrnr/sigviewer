#ifndef SIGNALGRAPHICSITEM_H
#define SIGNALGRAPHICSITEM_H

#include "base/sigviewer_user_types.h"
#include "command_executer.h"
#include "file_handling/event_manager.h"
#include "file_handling/channel_manager.h"
#include "gui/color_manager.h"
#include "gui/signal_view_settings.h"

#include <QGraphicsObject>
#include <QSharedPointer>
#include <QObject>

class QPoint;

namespace SigViewer_
{

class SignalEvent;
class SignalBrowserModel;
class EventGraphicsItem;

class SignalGraphicsItem : public QGraphicsObject
{
    Q_OBJECT
#if QT_VERSION >= 0x040600
    Q_INTERFACES(QGraphicsItem)
#endif
public:
    SignalGraphicsItem(QSharedPointer<SignalViewSettings const> signal_view_settings,
                       QSharedPointer<EventManager> event_manager,
                       QSharedPointer<CommandExecuter> command_executor,
                       ChannelManager const& channel_manager,
                       QSharedPointer<ColorManager const> color_manager,
                       ChannelID id,
                       SignalBrowserModel& model);

    virtual ~SignalGraphicsItem ();

    virtual QRectF boundingRect () const;

    void enableYGrid(bool enabled);
    void enableXGrid(bool enabled);

    float64 getYZoom() const;
    float64 getYOffset() const;
    float64 getYGridPixelIntervall() const;
    double getValueRangeFragment () const;
    QString getPhysicalDimensionString () const;

    void zoomIn();
    void zoomOut();
    void scale (double lower_value, double upper_value);
    void autoScale (ScaleMode auto_zoom_type);

public slots:
    void updateYGridIntervall();
    void setHeight (unsigned height);
    void setXGridInterval (unsigned interval);

signals:
    void mouseAtSecond (float64 sec);
    void mouseMoving (bool mouse_is_moving);
    void shifting (ChannelID channel_id);
    void updatedYGrid (ChannelID channel_id);

private:
    void scaleImpl (double min, double max);

    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event);
    virtual void contextMenuEvent (QGraphicsSceneContextMenuEvent * event);
    virtual void wheelEvent (QGraphicsSceneWheelEvent * event);

    void drawYGrid (QPainter* painter, QStyleOptionGraphicsItem const* option);
    void drawXGrid (QPainter* painter, QStyleOptionGraphicsItem const* option);

    QSharedPointer<SignalViewSettings const> signal_view_settings_;
    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<CommandExecuter> command_executor_;
    ChannelManager const& channel_manager_;
    QSharedPointer<ColorManager const> color_manager_;
    ChannelID id_;
    SignalBrowserModel& signal_browser_model_;

    float64 minimum_;
    float64 maximum_;

    float64 y_zoom_;
    float64 y_grid_pixel_intervall_;
    double value_range_fragment_;
    bool draw_y_grid_;
    bool draw_x_grid_;
    float64 y_offset_;
    int32 height_;
    uint32 width_;
    unsigned x_grid_interval_;

    bool shifting_;
    bool new_event_;
    EventGraphicsItem* created_event_item_;
    bool hand_tool_on_;
    QPointF move_start_point_;

    QSharedPointer<SignalEvent> new_signal_event_;
    int32 new_signal_event_reference_x_;
    QColor new_event_color_;
};

}

#endif // SIGNALGRAPHICSITEM_H
