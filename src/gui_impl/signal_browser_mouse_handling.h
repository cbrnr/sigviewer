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


#ifndef SIGNAL_BROWSER_MOUSE_HANDLING_H
#define SIGNAL_BROWSER_MOUSE_HANDLING_H



    class QMouseEvent;
    class QWheelEvent;

#include "gui/signal_visualisation_modes.h"
    class QGraphicsSceneMouseEvent;


namespace sigviewer
{

// signal browser mouse action
class SignalBrowserMouseHandling
{
public:
    enum Action
    {
        NO_ACTION,
        HAND_SCROLL_ACTION,
        ZOOM_WINDOW_ACTION,
        WHEEL_SCROLL_ACTION,
        ZOOM_ALL_CHANNELS_ACTION,
        ZOOM_CHANNEL_ACTION,
        SHIFT_CHANNEL_ACTION,
        NEW_EVENT_ACTION,
        SELECT_RESIZE_EVENT_ACTION,
        SHIFT_EVENT_TO_CHANNEL_ACTION,
        COPY_EVENT_TO_CHANNEL_ACTION
    };

    static Action getAction(QGraphicsSceneMouseEvent* e, SignalVisualisationMode mode);


};

} // namespace SigViewer_

#endif
