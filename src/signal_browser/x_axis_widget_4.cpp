#include "x_axis_widget_4.h"

#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFont>
#include <QTime>
#include <QPaintEvent>
#include <QGraphicsTextItem>
#include <QScrollBar>
#include <QHBoxLayout>
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
    length_in_sec_ (0),
    highlighting_enabled_ (false),
    time_to_highlight_ (0)
{
//    scene_ = new QGraphicsScene (this);
//    view_ = new QGraphicsView (scene_, this);
//    view_->horizontalScrollBar()->hide();
//    view_->verticalScrollBar()->hide();
//    view_->setMaximumHeight (30);
//    setMaximumHeight(30);
//
//    QHBoxLayout* layout = new QHBoxLayout ();
//    layout->setSpacing(0);
//    layout->setMargin(0);
//    layout->addWidget(view_);
//    this->setLayout(layout);
}

//-----------------------------------------------------------------------------
XAxisWidget::~XAxisWidget()
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
QSize XAxisWidget::sizeHint () const
{
    return QSize (100, 30);
}

//-----------------------------------------------------------------------------
void XAxisWidget::changeIntervall (float64 intervall)
{
    intervall_ = intervall;
    redrawScene ();
    update ();
}

//-----------------------------------------------------------------------------
void XAxisWidget::changeXStart(int32 x_start)
{
    x_start_ = x_start;
    update ();
}

//-----------------------------------------------------------------------------
void XAxisWidget::changePixelPerSec(float64 pixel_per_sec)
{
    pixel_per_sec_ = pixel_per_sec;
    redrawScene ();
    update ();
}

//-----------------------------------------------------------------------------
void XAxisWidget::changeTotalLengthInSecs (float64 seconds)
{
    length_in_sec_ = seconds;
    redrawScene ();
    update ();
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
//    view_->centerOn (x_start_, 0);
//    return;
    if (intervall_ < 1)
    {
        return; // invalid intervall
    }

    int32 x_end = x_start_ + width();
    int32 font_height = height() - 5;

    //if (!scale_pixmap_)
    //    redrawScalePixmap();
    QPainter p (this);
    //p.setClipping(true);
    //QPixmap bla(10, 10);
    //bla.fill (Qt::blue);
    //p.drawPixmap(0, 0, 10, 10, bla);
    //if (scale_pixmap_->height() >= 10 &&
    //    scale_pixmap_->width() >= 10)
    //{
    //    std::cout << "asdlfasödfkjaösfkdj" << std::endl;
    //    p.drawPixmap(0, 0, 10, 10, *scale_pixmap_);
    //}
    p.setPen (Qt::black);
    //p.drawLine (0, 4, width(), 4);
    //std::cout << "draw pixmap " << width() << "x" << height() << std::endl;
    //p.drawPixmap (0, 0, *scale_pixmap_, 0, 0, scale_pixmap_->width(), scale_pixmap_->height());

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
    //p.translate(-x_start_, 0);
    if (highlighting_enabled_)
    {
        QFont font;
        font.setPointSizeF (font.pointSizeF() * 1.5);
        font.setBold (true);
        p.setFont (font);
        QRect bounding_rect = p.boundingRect (0, 0, 0, font.pixelSize(), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(time_to_highlight_, 'f', 3));
        bounding_rect.moveTo (time_to_highlight_ * pixel_per_sec_, 5);
        p.fillRect (bounding_rect, Qt::white);
        p.setPen (Qt::black);
        p.drawText (bounding_rect, Qt::AlignLeft | Qt::AlignTop, QString::number(time_to_highlight_, 'f', 3));
        last_highlight_rect_ = bounding_rect;
    }
}

//-----------------------------------------------------------------------------
void XAxisWidget::redrawScene ()
{
    return;
    scene_->setSceneRect(0, 0, length_in_sec_ * pixel_per_sec_, 30);
    scene_->clear();
    scene_->addLine(0, 0, scene_->width(), 0);
    for (float64 float_x = 0;
         float_x < length_in_sec_ * pixel_per_sec_;
         float_x += intervall_)
    {
        QGraphicsTextItem* text = scene_->addText (QString::number(float_x / pixel_per_sec_));
        int32 x = float_x + 0.5;
        text->setPos (x, 5);
        scene_->addLine (x, 0, x, 5);
//        painter.drawLine(x, 0, x, 5);
//        painter.drawText((int32)(x - intervall_ / 2), 5, (int32)intervall_, font_height,
//                   Qt::AlignHCenter | Qt::AlignVCenter,
//                   QString::number(float_x / pixel_per_sec_));
    }

    return;
//    if (scale_pixmap_)
//        delete scale_pixmap_;
//    std::cout << "new pixmap " << length_in_sec_ * pixel_per_sec_ << "x" << height () << std::endl;
    //scale_pixmap_ = new QPixmap (length_in_sec_ * pixel_per_sec_, height ());
//    scale_pixmap_ = new QPixmap (4000, 50);
//    scale_pixmap_->fill (Qt::black);//palette().background().color());
//
//
//    QPainter painter (scale_pixmap_);
//    painter.setPen(Qt::black);
//
//    int32 font_height = height() - 5;
//
//    painter.drawLine(0, 0, scale_pixmap_->width(), 0);
//
//    for (float64 float_x = 0;
//         float_x < length_in_sec_ * pixel_per_sec_;
//         float_x += intervall_)
//    {
//        int32 x = float_x + 0.5;
//        painter.drawLine(x, 0, x, 5);
//        painter.drawText((int32)(x - intervall_ / 2), 5, (int32)intervall_, font_height,
//                   Qt::AlignHCenter | Qt::AlignVCenter,
//                   QString::number(float_x / pixel_per_sec_));
//    }

}


}
