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


#include "signal_browser_mouse_handling.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>

namespace SigViewer_
{

SignalBrowserMouseHandling::Action SignalBrowserMouseHandling::getAction(QGraphicsSceneMouseEvent* e, SignalVisualisationMode mode)
{
        // hand scroll
    if ((e->button() == Qt::MidButton  && e->modifiers() == Qt::NoModifier) ||
        (mode == MODE_HAND && e->button() == Qt::LeftButton))
    {
        return HAND_SCROLL_ACTION;
    }

    // shift channel
    if ((e->button() == Qt::MidButton && e->modifiers().testFlag(Qt::ShiftModifier)) ||
        (mode == MODE_VIEW_OPTIONS && e->button() == Qt::LeftButton))
    {
        return SHIFT_CHANNEL_ACTION;
    }

    // new event
    if (e->button() == Qt::LeftButton && mode == MODE_NEW)
    {
        return NEW_EVENT_ACTION;
    }

    // select or resize event
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::NoModifier &&
        mode == MODE_POINTER)
    {
        return SELECT_RESIZE_EVENT_ACTION;
    }

    // shift event to channel
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier &&
        mode == MODE_POINTER)
    {
        return SHIFT_EVENT_TO_CHANNEL_ACTION;
    }

    // copy event to channel
    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier
        && mode == MODE_POINTER)
    {
        return COPY_EVENT_TO_CHANNEL_ACTION;
    }

    return NO_ACTION;
}

} // namespace SigViewer_
