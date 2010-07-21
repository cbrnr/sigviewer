#include "signal_view_settings.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
SignalViewSettings::SignalViewSettings (QSharedPointer<ChannelManager> channel_manager)
    : pixels_per_sample_ (1),
      channel_manager_ (channel_manager)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void SignalViewSettings::setPixelsPerSample (float pixels_per_sample)
{
    pixels_per_sample_ = pixels_per_sample;
    emit pixelsPerSampleChanged ();
}

}
