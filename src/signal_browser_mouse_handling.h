// signal_browser_mouse_handling.h

#ifndef SIGNAL_BROWSER_MOUSE_HANDLING_H
#define SIGNAL_BROWSER_MOUSE_HANDLING_H



    class QMouseEvent;
    class QWheelEvent;

    #include "signal_browser/signal_browser_model_4.h"
    class QGraphicsSceneMouseEvent;


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

    static Action getAction(QGraphicsSceneMouseEvent* e, SignalBrowserModel::Mode mode);


};

} // namespace BioSig_

#endif
