// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


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
};

}

#endif // X_AXIS_WIDGET_H
