#ifndef QT4_PORTED
// x_axis_widget.cpp

#include "x_axis_widget.h"
#include "signal_browser_model.h"
#include "signal_browser.h"
#include "smart_canvas/smart_canvas_view.h"

#include <math.h>

#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QSettings>
#include <QPalette>

namespace BioSig_
{

// axis color
QColor XAxisWidget::axis_color_("#e0e0e0");

// constructor
XAxisWidget::XAxisWidget(SignalBrowserModel& model, SignalBrowser* browser)
: QWidget(browser),
  signal_browser_model_(model),
  signal_browser_(browser)
{
    QPalette palette;
    palette.setColor(backgroundRole(), axis_color_);
    setPalette(palette);
    QPixmap dummy(1,1);
    QPainter p(&dummy);
    QRect bounding = p.boundingRect(0, 0, 500, 500,
                                    Qt::AlignHCenter | Qt::AlignVCenter,
                                    "0123456789.-");
    setMinimumHeight(5 + bounding.height());
}

// get axis color
const QColor& XAxisWidget::getAxisColor()
{
    return axis_color_;
}

// load settings
void XAxisWidget::loadSettings()
{
    QSettings settings("SigViewer");
    axis_color_.setNamedColor(settings.value("XAxisWidget/axis_color",
                                             axis_color_.name()).toString());
}

// save settings
void XAxisWidget::saveSettings()
{
    QSettings settings("SigViewer");
    settings.setValue("XAxisWidget/axis_color", axis_color_.name());
}
// paint event
void XAxisWidget::paintEvent(QPaintEvent*)
{
    float64 intervall = signal_browser_model_.getXGridPixelIntervall();

    if (intervall < 1)
    {
        return; // invalid intervall
    }

    float64 pixel_per_sec = signal_browser_model_.getPixelPerSec();
    int32 x_start = signal_browser_->getCanvasView()->contentsX();
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
    }

}

} // namespace BioSig_

#endif
