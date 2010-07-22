#include "signal_view_settings.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
SignalViewSettings::SignalViewSettings (QSharedPointer<ChannelManager> channel_manager)
    : pixels_per_sample_ (1),
      channel_manager_ (channel_manager)
{
    grid_fragmentation_[Qt::Horizontal] = 10;
    grid_fragmentation_[Qt::Vertical] = 4;
}

//-----------------------------------------------------------------------------
void SignalViewSettings::setPixelsPerSample (float pixels_per_sample)
{
    pixels_per_sample_ = pixels_per_sample;
    emit pixelsPerSampleChanged ();
}


//-------------------------------------------------------------------------
void SignalViewSettings::setGridFragmentation (Qt::Orientation orientation, int fragmentation)
{
    grid_fragmentation_[orientation] = fragmentation;
    emit gridFragmentationChanged ();
}

}
