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


#include "signal_view_settings.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
SignalViewSettings::SignalViewSettings (ChannelManager const& channel_manager)
    : pixels_per_sample_ (1),
      channel_overlapping_ (0),
      channel_heigth_in_pixels_ (20),
      channel_manager_ (channel_manager)
{
    grid_fragmentation_[Qt::Horizontal] = 10;
    grid_fragmentation_[Qt::Vertical] = 4;
}

//-----------------------------------------------------------------------------
void SignalViewSettings::setPixelsPerSample (float pixels_per_sample)
{
    float old_pixel_per_sample = pixels_per_sample_;
    pixels_per_sample_ = pixels_per_sample;
    if (old_pixel_per_sample != pixels_per_sample_)
        emit pixelsPerSampleChanged ();
}

//-----------------------------------------------------------------------------
void SignalViewSettings::setChannelOverlapping (float channel_overlapping)
{
    float old_channel_overlapping = channel_overlapping_;
    channel_overlapping_ = channel_overlapping;
    if (old_channel_overlapping != channel_overlapping_)
        emit channelOverlappingChanged ();
}


//-------------------------------------------------------------------------
void SignalViewSettings::setChannelHeight (int channel_heigth_in_pixels)
{
    int old_channel_height = channel_heigth_in_pixels_;
    channel_heigth_in_pixels_ = channel_heigth_in_pixels;
    if (old_channel_height != channel_heigth_in_pixels_)
    {
        emit channelHeightChanged ();
        emit channelHeightChanged (channel_heigth_in_pixels_);
    }
}

//-------------------------------------------------------------------------
void SignalViewSettings::setGridFragmentation (Qt::Orientation orientation, int fragmentation)
{
    int old_fragmentation = grid_fragmentation_[orientation];
    grid_fragmentation_[orientation] = fragmentation;
    if (old_fragmentation != fragmentation)
        emit gridFragmentationChanged ();
}

}
