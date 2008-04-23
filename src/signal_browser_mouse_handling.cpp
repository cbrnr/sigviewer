// signal_browser_mouse_handling.cpp



#include "signal_browser_mouse_handling.h"



#include <QMouseEvent>

#include <QWheelEvent>



namespace BioSig_

{



// get action

SignalBrowserMouseHandling::Action SignalBrowserMouseHandling::getAction(

                                                QMouseEvent* e,

                                                SignalBrowserModel::Mode mode)

{

    // hand scroll

    if (e->button() == Qt::MidButton  && e->modifiers() == Qt::NoModifier ||

        mode == SignalBrowserModel::MODE_HAND && e->button() == Qt::LeftButton)

    {

        return HAND_SCROLL_ACTION;

    }



    // zoom window

    if (mode == SignalBrowserModel::MODE_ZOOM && e->button() == Qt::LeftButton)

    {

        return ZOOM_WINDOW_ACTION;

    }



    // shift channel

    if (e->button() == Qt::MidButton && e->state() & Qt::ShiftButton ||

        mode == SignalBrowserModel::MODE_SHIFT_SIGNAL && e->button() == Qt::LeftButton)

    {

        return SHIFT_CHANNEL_ACTION;

    }



    // new event

    if (e->button() == Qt::LeftButton && mode == SignalBrowserModel::MODE_NEW)

    {

        return NEW_EVENT_ACTION;   

    }



    // select or resize event

    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::NoModifier &&

        mode == SignalBrowserModel::MODE_POINTER)

    {

        return SELECT_RESIZE_EVENT_ACTION;

    }



    // shift event to channel

    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier &&

        mode == SignalBrowserModel::MODE_POINTER)

    {

        return SHIFT_EVENT_TO_CHANNEL_ACTION;

    }



    // copy event to channel

    if (e->button() == Qt::LeftButton && e->modifiers() == Qt::ControlModifier

        && mode == SignalBrowserModel::MODE_POINTER)

    {

        return COPY_EVENT_TO_CHANNEL_ACTION;

    }



    return NO_ACTION;

}



// get action

SignalBrowserMouseHandling::Action SignalBrowserMouseHandling::getAction(

                                            QWheelEvent* e,

                                            SignalBrowserModel::Mode)

{

    // wheel scroll

    if (e->modifiers() == Qt::NoModifier)

    {

        return WHEEL_SCROLL_ACTION;

    }



    // zoom all channels

    if (e->modifiers() == Qt::ShiftModifier)

    {

        return ZOOM_ALL_CHANNELS_ACTION;   

    }



    // zoom channel

    if (e->modifiers() == Qt::ControlModifier)

    {

        return ZOOM_CHANNEL_ACTION;   

    }



    return NO_ACTION;

}



} // namespace BioSig_

