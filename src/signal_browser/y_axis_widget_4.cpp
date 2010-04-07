#include "y_axis_widget_4.h"

#include "signal_graphics_item.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QProgressDialog>

#include <cmath>

namespace BioSig_
{

//-----------------------------------------------------------------------------
YAxisWidget::YAxisWidget (QWidget* parent)
  : QWidget (parent),
    pixmap_ (0),
    signal_height_ (0),
    signal_spacing_ (1),
    y_start_ (0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
YAxisWidget::~YAxisWidget ()
{
    if (pixmap_)
        delete pixmap_;
}

//-----------------------------------------------------------------------------
QSize YAxisWidget::sizeHint () const
{
    return QSize (70, 0);
}

//-----------------------------------------------------------------------------
void YAxisWidget::addChannel(int32 channel_nr, SignalGraphicsItem const* const signal_item)
{
    if (signal_item)
    {
        channel_nr2signal_graphics_item_[channel_nr] = signal_item;
        update ();
    }
}

//-----------------------------------------------------------------------------
void YAxisWidget::removeChannel(int32 channel_nr)
{
    QMap<int32, SignalGraphicsItem const*>::iterator it = channel_nr2signal_graphics_item_.find(channel_nr);

    if (it != channel_nr2signal_graphics_item_.end())
    {
        channel_nr2signal_graphics_item_.erase(it);
        update ();
    }
}

//-----------------------------------------------------------------------------
void YAxisWidget::changeSignalHeight (uint32 signal_height)
{
    signal_height_ = signal_height;
    repaintPixmap ();
    update ();
}

//-----------------------------------------------------------------------------
void YAxisWidget::changeSignalSpacing (unsigned signal_spacing)
{
    signal_spacing_ = signal_spacing;
    repaintPixmap ();
    update ();
}

//-----------------------------------------------------------------------------
void YAxisWidget::changeYStart (int y_start)
{
    y_start_ = y_start;
    update ();
}

//-----------------------------------------------------------------------------
void YAxisWidget::updateChannel (ChannelID channel)
{
    // TODO: repaint selective and not all!!
    repaintPixmap (channel);
    update ();
}

//-----------------------------------------------------------------------------
void YAxisWidget::updateAllChannels ()
{
    repaintPixmap ();
    update ();
}


//-----------------------------------------------------------------------------
void YAxisWidget::paintEvent(QPaintEvent*)
{
    if (!pixmap_)
        return;

    QPainter painter (this);
    unsigned y = 0;
    if (pixmap_->height() < height())
        y = (height() - pixmap_->height()) / 2;
    painter.drawPixmap (0, y, *pixmap_, 0, y_start_, pixmap_->width(), pixmap_->height());
}

//-------------------------------------------------------------------
void YAxisWidget::repaintPixmap (int32 channel)
{
    return;
    if (!isVisible())
        return;
    if (channel_nr2signal_graphics_item_.size() == 0)
        return;

    float64 intervall = signal_height_ + signal_spacing_;
    unsigned height = intervall * channel_nr2signal_graphics_item_.size ();
    unsigned w = width ();

    if (pixmap_)
    {
        if (pixmap_->height() < height ||
            pixmap_->width() < w)
        {
            pixmap_ = new QPixmap (w, height);
            delete pixmap_;
            pixmap_ = new QPixmap (w, height);
        }
    }
    else
        pixmap_ = new QPixmap (w, height);

    QPainter painter (pixmap_);
    painter.setPen(Qt::black);
    painter.setClipping (true);

    QMap<int32, SignalGraphicsItem const*>::const_iterator iter = channel_nr2signal_graphics_item_.begin();
    QMap<int32, SignalGraphicsItem const*>::const_iterator end_iter = channel_nr2signal_graphics_item_.end();
    float y_start = 0;

    if (channel >= 0)
    {
        iter = channel_nr2signal_graphics_item_.find (channel);
        end_iter = iter;
        ++end_iter;
        QMap<int32, SignalGraphicsItem const*>::const_iterator y_calc_iter = iter;
        while (y_calc_iter != channel_nr2signal_graphics_item_.begin())
        {
            --y_calc_iter;
            y_start += intervall;
        }
    }

    // waldesel: this dialog is needed because drawing of strings
    //           is very slow... I hope with qt4.7 this is not longer needed!
//    QProgressDialog progress;
//    progress.setMaximum (channel_nr2signal_graphics_item_.size());
//    progress.setMinimum (0);
//    progress.setModal (true);
//    progress.setLabelText (tr("Drawing Y Axis "));
//    progress.show ();

    for (;
         iter != end_iter;
         y_start += intervall, ++iter)
    {
//        progress.setValue (iter.key ());
        painter.setClipRect (0, y_start, w, signal_height_+1);
        painter.fillRect (0, y_start, w, signal_height_+1, palette().background().color());

        painter.drawLine (0, y_start,
                          w - 1, y_start);
        painter.drawLine (0, y_start + signal_height_,
                          w - 1, y_start + signal_height_);


        float64 zero_y = y_start + (static_cast<float32>(signal_height_) / 2.0f);
        painter.translate (0, zero_y);
        float64 y_grid_pixel_intervall = iter.value()->getYGridPixelIntervall();

        float64 offset = iter.value()->getYOffset();

        for (float64 value_y = offset;
             value_y < signal_height_ / 2;
             value_y += y_grid_pixel_intervall)
        {
            if (value_y > -static_cast<int>(signal_height_ / 2))
            {
                painter.drawLine (w - 5, value_y, w - 1, value_y);
                painter.drawText(0, value_y - 20, w - 10, 40,
                                 Qt::AlignRight | Qt::AlignVCenter,
                                 QString::number (qRound((offset-value_y) / iter.value()->getYZoom()
                                            * 100) / 100.0));
            }
        }
        for (float64 value_y = offset - y_grid_pixel_intervall;
             value_y > -static_cast<int>(signal_height_ / 2);
             value_y -= y_grid_pixel_intervall)
        {
            if (value_y < signal_height_ / 2)
            {
                painter.drawLine (w - 5, value_y, w - 1, value_y);
                painter.drawText(0, value_y - 20, w - 10, 40,
                                 Qt::AlignRight | Qt::AlignVCenter,
                                 QString::number (qRound((offset-value_y) / iter.value()->getYZoom()
                                            * 100) / 100.0));
            }
        }

        painter.translate (0, -zero_y);
    }
//    progress.setValue (progress.maximum ());
}



}
