#ifndef EVENT_GRAPHICS_ITEM_H
#define EVENT_GRAPHICS_ITEM_H


#include <QGraphicsItem>
#include "../base/user_types.h"

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

    virtual QRectF boundingRect () const;

protected:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );


};

}
}

#endif // EVENT_GRAPHICS_ITEM_H
