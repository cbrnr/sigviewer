#ifndef SIGNALGRAPHICSITEM_H
#define SIGNALGRAPHICSITEM_H

#include "../base/user_types.h"

#include <QGraphicsItem>
#include <QSharedPointer>

class QPoint;

namespace BioSig_
{

class SignalBuffer;
class SignalChannel;
class SignalEvent;
class SignalBrowserModel;
class SignalBrowserView;
class EventGraphicsItem;

class SignalGraphicsItem : public QGraphicsItem
{
public:
    SignalGraphicsItem(SignalBuffer& buffer, const SignalChannel& channel,
                     SignalBrowserModel& model, SignalBrowserView* browser);

    virtual ~SignalGraphicsItem ();

    void setHeight (int32 height);
    virtual QRectF boundingRect () const;

    void getRangeFromBuffer(float64 factor);
    void updateYGridIntervall();
    void enableYGrid(bool enabled);
    void enableXGrid(bool enabled);
    QString const &getLabel () const;

    float64 getMaximum();
    float64 getMinimum();
    float64 getYZoom();
    float64 getYOffset();
    float64 getYGridPixelIntervall();

    void zoomIn();
    void zoomOut();
    void autoScale(ScaleMode auto_zoom_type = MAX_TO_MAX);


protected:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event);
    virtual void contextMenuEvent (QGraphicsSceneContextMenuEvent * event);

private:
    void drawYAxis (QPainter * painter, const QStyleOptionGraphicsItem * option);

    SignalBuffer& signal_buffer_;
    const SignalChannel& signal_channel_;
    SignalBrowserModel& signal_browser_model_;
    SignalBrowserView* signal_browser_;

    float64 minimum_;
    float64 maximum_;

    float64 y_zoom_;
    float64 y_grid_pixel_intervall_;
    bool draw_y_grid_;
    bool draw_x_grid_;
    float64 y_offset_; // middle
    int32 height_;


    bool shifting_;
    bool new_event_;
    QSharedPointer<EventGraphicsItem> created_event_item_;
    bool hand_tool_on_;
    QPointF move_start_point_;

    QSharedPointer<SignalEvent> new_signal_event_;
    QColor new_event_color_;

    static float64 prefered_pixel_per_sample_;


};

}

#endif // SIGNALGRAPHICSITEM_H
