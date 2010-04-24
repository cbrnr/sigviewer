#include "signal_visualisation_model.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
SignalVisualisationModel::SignalVisualisationModel (float32 sample_rate,
                                                    std::set<EventType> const& shown_types)
    : pixel_per_sample_ (1),
      sample_rate_ (sample_rate),
      mode_ (MODE_HAND),
      event_creation_type_ (1),
      shown_event_types_ (shown_types)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setMode (SignalVisualisationMode mode)
{
    mode_ = mode;
    modeChanged (mode_);
}

//-----------------------------------------------------------------------------
SignalVisualisationMode SignalVisualisationModel::getMode () const
{
    return mode_;
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setPixelPerSample (float32 pixel_per_sample)
{
    pixel_per_sample_ = pixel_per_sample;
    emit pixelPerSampleChanged (pixel_per_sample_, sample_rate_);
}

//-----------------------------------------------------------------------------
float32 SignalVisualisationModel::getPixelPerSample () const
{
    return pixel_per_sample_;
}

//-----------------------------------------------------------------------------
float32 SignalVisualisationModel::getSampleRate () const
{
    return sample_rate_;
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


//-----------------------------------------------------------------------------
void SignalVisualisationModel::setActualEventCreationType (EventType type)
{
    event_creation_type_ = type;
}



}
