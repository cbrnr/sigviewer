#include "x_axis_widget_4.h"

#include <QPainter>
#include <cmath>

namespace BioSig_
{

//-----------------------------------------------------------------------------
XAxisWidget::XAxisWidget (QWidget* parent)
  : QWidget (parent),
    intervall_ (10),
    x_start_ (0),
    pixel_per_sec_ (2)
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
    /*    float64 intervall = signal_browser_model_.getXGridPixelIntervall();

        if (intervall < 1)
        {
            return; // invalid intervall
        }

        float64 pixel_per_sec = signal_browser_model_.getPixelPerSec();
        int32 x_start = signal_browser_->getVisibleX();
        int32 x_end = x_start + width();
        int32 font_height = height() - 5;

        QPainter p(this);
        p.translate(-x_start, 0);
        p.drawLine(x_start, 0, x_end, 0);
        float64 float_x_start = floor((x_start + intervall / 2) / intervall) *
                               intervall;

        float64 float_x_end = ceil((x_end - intervall / 2) / intervall) *
                              intervall + intervall / 2;

        for (float32 float_x = float_x_start;
             float_x < float_x_end;
             float_x += intervall)
        {
            int32 x = (int32)(float_x + 0.5);
            p.drawLine(x, 0, x, 5);
            p.drawText((int32)(x - intervall / 2), 5, (int32)intervall, font_height,
                       Qt::AlignHCenter | Qt::AlignVCenter,
                       QString::number(float_x / pixel_per_sec));
        }*/

}

}
