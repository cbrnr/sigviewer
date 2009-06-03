#ifndef EVENT_GRAPHICS_ITEM_H
#define EVENT_GRAPHICS_ITEM_H


#include "../base/user_types.h"

#include <QGraphicsItem>
#include <QColor>

namespace BioSig_
{

class SignalBuffer;

namespace PortingToQT4_
{

class SignalBrowserModel;
class SignalBrowserView;


class EventGraphicsItem : public QGraphicsItem
{
public:
    EventGraphicsItem(uint32 id, SignalBuffer& buffer, SignalBrowserModel& model,
                    SignalBrowserView* browser);

    virtual ~EventGraphicsItem ();

    void setSize (int32 width, int32 height);
    void updateColor ();

    virtual QRectF boundingRect () const;

protected:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    SignalBrowserModel& signal_browser_model_;
    SignalBuffer& signal_buffer_;
    QColor color_;
    uint32 id_;

    int32 height_;
    int32 width_;


};

}
}

#endif // EVENT_GRAPHICS_ITEM_H
