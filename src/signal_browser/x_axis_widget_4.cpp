#include "x_axis_widget_4.h"

#include <QPainter>
#include <QFont>
#include <cmath>

#include <iostream>

namespace BioSig_
{

//-----------------------------------------------------------------------------
XAxisWidget::XAxisWidget (QWidget* parent)
  : QWidget (parent),
    intervall_ (10),
    x_start_ (0),
    pixel_per_sec_ (2),
    highlighting_enabled_ (false),
    time_to_highlight_ (0)
{

}

//-----------------------------------------------------------------------------
XAxisWidget::~XAxisWidget()
{

}


//-----------------------------------------------------------------------------
QSize XAxisWidget::sizeHint () const
{
    return QSize (100, 50);
}

//-----------------------------------------------------------------------------
void XAxisWidget::changeIntervall (float64 intervall)
{
    intervall_ = intervall;
    repaint ();
}

//-----------------------------------------------------------------------------
void XAxisWidget::changeXStart(int32 x_start)
{
    x_start_ = x_start;
    repaint ();
}

//-----------------------------------------------------------------------------
void XAxisWidget::changePixelPerSec(float64 pixel_per_sec)
{
    pixel_per_sec_ = pixel_per_sec;
    repaint ();
}

//-----------------------------------------------------------------------------
void XAxisWidget::changeHighlightTime (float64 time_to_highlight)
{
    time_to_highlight_ = time_to_highlight;// - (x_start_ / pixel_per_sec_);
    update ();
}

//-----------------------------------------------------------------------------
void XAxisWidget::enableHighlightTime (bool highlighting_enabled)
{
    highlighting_enabled_ = highlighting_enabled;
    update ();
}


//-----------------------------------------------------------------------------
void XAxisWidget::paintEvent(QPaintEvent*)
{
    if (intervall_ < 1)
    {
        return; // invalid intervall
    }

    int32 x_end = x_start_ + width();
    int32 font_height = height() - 5;

    QPainter p(this);
    p.translate(-x_start_, 0);
    p.drawLine(x_start_, 0, x_end, 0);
    float64 float_x_start = floor((x_start_ + intervall_ / 2) / intervall_) *
                           intervall_;

    float64 float_x_end = ceil((x_end - intervall_ / 2) / intervall_) *
                          intervall_ + intervall_ / 2;

    for (float32 float_x = float_x_start;
         float_x < float_x_end;
         float_x += intervall_)
    {
        int32 x = (int32)(float_x + 0.5);
        p.drawLine(x, 0, x, 5);
        p.drawText((int32)(x - intervall_ / 2), 5, (int32)intervall_, font_height,
                   Qt::AlignHCenter | Qt::AlignVCenter,
                   QString::number(float_x / pixel_per_sec_));
    }
    if (highlighting_enabled_)
    {
        QFont font;
        font.setPointSizeF (font.pointSizeF() * 1.5);
        font.setBold (true);
        p.setFont (font);
        QRect bounding_rect = p.boundingRect (0, 0, 200, font.pixelSize(), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(time_to_highlight_));
        bounding_rect.moveTo (time_to_highlight_ * pixel_per_sec_, 5);
        p.fillRect (bounding_rect, Qt::white);
        p.setPen (Qt::black);
        p.drawText (bounding_rect, Qt::AlignLeft | Qt::AlignTop, QString::number(time_to_highlight_));
        last_highlight_rect_ = bounding_rect;
    }
}

}
