// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef X_AXIS_WIDGET_H
#define X_AXIS_WIDGET_H

#include "base/sigviewer_user_types.h"
#include "gui/signal_view_settings.h"

#include <QWidget>
#include <QSharedPointer>

namespace sigviewer
{

class XAxisWidget : public QWidget
{
    Q_OBJECT
public:
    XAxisWidget (QSharedPointer<SignalViewSettings const> signal_view_settings,
                 QWidget* parent);
    virtual ~XAxisWidget();
    virtual QSize sizeHint () const;

public slots:
    void changeXStart (int32 x_start);
    void changeHighlightTime (float64 time_to_highlight);
    void enableHighlightTime (bool highlighting_enabled);
    void changeLabelColor(QColor labelColor);

private:
    virtual void paintEvent(QPaintEvent*);
    virtual void contextMenuEvent (QContextMenuEvent* event);
    virtual void timerEvent (QTimerEvent* event);

    QSharedPointer<SignalViewSettings const> signal_view_settings_;
    float64 intervall_;
    int32 x_start_;
    float64 pixel_per_sec_;
    float64 length_in_sec_;
    bool highlighting_enabled_;
    float64 time_to_highlight_;
    QRect last_highlight_rect_;

    int highlight_timer_;
    QColor label_color_;
};

}

#endif // X_AXIS_WIDGET_H
