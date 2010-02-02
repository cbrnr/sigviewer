// label_widget.cpp

#include "label_widget.h"
#include "signal_browser/signal_browser_view.h"

#include <math.h>

#include <QPalette>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QSettings>

#define min(a, b) ((a) < (b) ? (a) : (b))

namespace BioSig_
{

// axis color
QColor LabelWidget::axis_color_("#e0e0e0");

// constructor
LabelWidget::LabelWidget(SignalBrowserModel& model, SignalBrowserView* browser)
: QWidget(browser),
  signal_browser_model_(model),
  signal_browser_(browser)

{
    QPalette palette;
    palette.setColor(backgroundRole(), axis_color_);
    setPalette(palette);
}

// get axis color
const QColor& LabelWidget::getAxisColor()
{
    return axis_color_;
}

// load settings
void LabelWidget::loadSettings()
{
    QSettings settings("SigViewer");
    axis_color_.setNamedColor(settings.value("LabelWidget/axis_color",
                                             axis_color_.name()).toString());
}

// save settings
void LabelWidget::saveSettings()
{
    QSettings settings("SigViewer");
    settings.setValue("LabelWidget/axis_color", axis_color_.name());
}

// paint event
void LabelWidget::paintEvent(QPaintEvent*)
{
    int32 signal_height = signal_browser_model_.getSignalHeight();
    int32 signal_spacing = signal_browser_model_.getSignalSpacing();
    float64 intervall = signal_height + signal_spacing;
    int32 y_start = signal_browser_->getVisibleY();
    int32 y_end = y_start + height();
    int32 w = width();

    QPainter p(this);
    p.translate(0, -y_start);
    p.drawLine(0, y_start, 0, y_end);

    // labels
    float64 float_y_start = floor(y_start / intervall) * intervall;
    float64 float_y_end = ceil(y_end / intervall) * intervall;
    QMap<int32, QString>::iterator iter = channel_nr2label_.begin();

    for (float32 float_y = signal_height / 2;
         float_y < float_y_end && iter != channel_nr2label_.end();
         float_y += intervall, iter++)
    {
        if (float_y > float_y_start)
        {
            int32 y = (int32)(float_y + 0.5);
            p.drawText(0, (int32)(y - intervall /2) , w - 10, (int32)intervall,
                       Qt::AlignHCenter | Qt::AlignVCenter, iter.value());
        }
    }

    // markers
    float_y_start = ceil((y_start - signal_height - signal_spacing / 2) /
                         intervall) *
                    intervall + signal_height + signal_spacing / 2;

    float_y_end = min(floor((y_end + signal_height + signal_spacing / 2) /
                            intervall), channel_nr2label_.size()) *
                  intervall - signal_spacing / 2 + intervall / 2;

    for (float32 float_y = float_y_start;
         float_y < float_y_end;
         float_y += intervall)
    {
        int32 y = (int32)(float_y + 0.5);
        p.drawLine(0, y, w - 1, y);
    }
}

// add channel
void LabelWidget::addChannel(int32 channel_nr, const QString& label)
{
    channel_nr2label_[channel_nr] = label;
    QPixmap dummy(1,1);
    QPainter p(&dummy);
    QRect bounding = p.boundingRect(0, 0, 500, 500,
                                    Qt::AlignHCenter | Qt::AlignVCenter, label);
    int32 max_width = minimumWidth() - 10;
    max_width = bounding.width() > max_width ? bounding.width() : max_width;
    setMinimumWidth(10 + max_width);
}

// remove channel
void LabelWidget::removeChannel(int32 channel_nr)
{
    channel_nr2label_.erase(channel_nr2label_.find(channel_nr));
    int32 max_width = -1;
    QPixmap dummy(1,1);
    QPainter p(&dummy);

    for (QMap<int32, QString>::iterator iter = channel_nr2label_.begin();
         iter != channel_nr2label_.end();
         iter++)
    {
        QRect bounding = p.boundingRect(0, 0, 200, p.font().pixelSize(),
                                        Qt::AlignHCenter | Qt::AlignVCenter,
                                        iter.value());

        max_width = max_width < bounding.width() ? bounding.width() : max_width;
    }

    setMinimumWidth(10 + max_width);
}

////-----------------------------------------------------------------------------
//QSize LabelWidget::sizeHint () const
//{
//    return QSize (100,0);
//}

} // namespace BioSig_

