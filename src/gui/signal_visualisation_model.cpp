#include "signal_visualisation_model.h"

#include <QDebug>

namespace BioSig_
{

//-----------------------------------------------------------------------------
SignalVisualisationModel::SignalVisualisationModel (std::set<EventType> const& shown_types)
    : pixel_per_sample_ (1),
      mode_ (MODE_HAND),
      event_creation_type_ (1),
      shown_event_types_ (shown_types),
      signal_height_ (10),
      scale_mode_ (MIN_TO_MAX),
      selected_channel_ (UNDEFINED_CHANNEL),
      info_widget_ (0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setMode (SignalVisualisationMode mode)
{
    mode_ = mode;
    emit modeChanged (mode_);
    modeChangedImpl (mode_);
}

//-----------------------------------------------------------------------------
SignalVisualisationMode SignalVisualisationModel::getMode () const
{
    return mode_;
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setInfoWidget (QWidget* info_widget)
{
    info_widget_ = info_widget;
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setPixelPerSample (float pixel_per_sample)
{
    pixel_per_sample_ = pixel_per_sample;
    emit pixelPerSampleChanged (pixel_per_sample_, getChannelManager()->getSampleRate());
    update ();
}

//-----------------------------------------------------------------------------
float SignalVisualisationModel::getPixelPerSample () const
{
    return pixel_per_sample_;
}

//-------------------------------------------------------------------------
unsigned SignalVisualisationModel::getSignalHeight () const
{
    return signal_height_;
}

//-------------------------------------------------------------------------
void SignalVisualisationModel::setSignalHeight (unsigned height)
{
    signal_height_ = height;
    qDebug() << "SignalVisualisationModel::signal_height_ = " << signal_height_;
    if (signal_height_ == 0)
        signal_height_ = 100;
    emit signalHeightChanged (signal_height_);
    update();
}

//-------------------------------------------------------------------------
ChannelID SignalVisualisationModel::getSelectedChannel () const
{
    return selected_channel_;
}

//-------------------------------------------------------------------------
void SignalVisualisationModel::selectChannel (ChannelID channel)
{
    selected_channel_ = channel;
}


//-----------------------------------------------------------------------------
EventID SignalVisualisationModel::getSelectedEvent () const
{
    return UNDEFINED_EVENT_ID;
}


//-----------------------------------------------------------------------------
std::set<EventType> SignalVisualisationModel::getShownEventTypes () const
{
    return shown_event_types_;
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setShownEventTypes (std::set<EventType> const& event_types)
{
    shown_event_types_ = event_types;
    emit shownEventTypesChanged (shown_event_types_);
    shownEventTypesChangedImpl ();
}

//-----------------------------------------------------------------------------
EventType SignalVisualisationModel::getActualEventCreationType () const
{
    return event_creation_type_;
}

//-------------------------------------------------------------------------
void SignalVisualisationModel::setAutoScaleMode (ScaleMode scale_mode)
{
    scale_mode_ = scale_mode;
}

//-------------------------------------------------------------------------
ScaleMode SignalVisualisationModel::getAutoScaleMode () const
{
    return scale_mode_;
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setActualEventCreationType (EventType type)
{
    event_creation_type_ = type;
}

//-----------------------------------------------------------------------------
QWidget* SignalVisualisationModel::infoWidget ()
{
    return info_widget_;
}

}
