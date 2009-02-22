// signal_browser_mouse_handling.h

#ifndef SIGNAL_BROWSER_MOUSE_HANDLING_H
#define SIGNAL_BROWSER_MOUSE_HANDLING_H

#include "signal_browser_model.h"

class QMouseEvent;
class QWheelEvent;

namespace BioSig_
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

    static Action getAction(QMouseEvent* e, SignalBrowserModel::Mode mode);
    static Action getAction(QWheelEvent* e, SignalBrowserModel::Mode mode);
};

} // namespace BioSig_

#endif
