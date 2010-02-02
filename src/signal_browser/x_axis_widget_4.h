#ifndef X_AXIS_WIDGET_H
#define X_AXIS_WIDGET_H

#include "../base/user_types.h"

#include <QWidget>

namespace BioSig_
{

class XAxisWidget : public QWidget
{
    Q_OBJECT

public:
    XAxisWidget (QWidget* parent);
    virtual ~XAxisWidget();
    virtual QSize sizeHint () const;

public slots:
    void changeIntervall (float64 intervall);
    void changeXStart (int32 x_start);
    void changePixelPerSec (float64 pixel_per_sec);
    void changeHighlightTime (float64 time_to_highlight);
    void enableHighlightTime (bool highlighting_enabled);

protected:
    virtual void paintEvent(QPaintEvent* event = 0);

    float64 intervall_;
    int32 x_start_;
    float64 pixel_per_sec_;
    bool highlighting_enabled_;
    float64 time_to_highlight_;
    QRect last_highlight_rect_;
};

}

#endif // X_AXIS_WIDGET_H
