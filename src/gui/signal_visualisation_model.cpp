#include "signal_visualisation_model.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
SignalVisualisationModel::SignalVisualisationModel (float32 sample_rate)
    : pixel_per_sample_ (1),
      sample_rate_ (sample_rate),
      mode_ (MODE_HAND)
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
float32 SignalVisualisationModel::getPixelPerSample ()
{
    return pixel_per_sample_;
}

//-----------------------------------------------------------------------------
float32 SignalVisualisationModel::getSampleRate ()
{
    return sample_rate_;
}

//-----------------------------------------------------------------------------
EventID SignalVisualisationModel::getSelectedEvent () const
{
    return UNDEFINED_EVENT_ID;
}



}
