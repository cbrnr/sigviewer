#include "x_axis_graphics_item.h"

#include <QPainter>

namespace BioSig_
{

namespace PortingToQT4_
{

//-----------------------------------------------------------------------------
XAxisWidget::XAxisWidget()
{
}

//-----------------------------------------------------------------------------
XAxisWidget::~XAxisWidget()
{
}


//-----------------------------------------------------------------------------
QSize XAxisWidget::sizeHint () const
{
    return QSize (100, 30);
}

//-----------------------------------------------------------------------------
void XAxisWidget::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setPen(Qt::black);
    p.drawLine(0,0,100,30);
}

}


}
