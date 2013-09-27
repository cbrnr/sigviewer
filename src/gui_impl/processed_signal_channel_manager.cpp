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


#include "processed_signal_channel_manager.h"

namespace SigViewer_
{

//-------------------------------------------------------------------------
ProcessedSignalChannelManager::ProcessedSignalChannelManager(float64 sample_rate, unsigned length, QObject* parent)
    : QObject (parent),
      sample_rate_ (sample_rate),
      length_ (length)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
void ProcessedSignalChannelManager::addChannel (ChannelID id, QSharedPointer<DataBlock const> data_block,
                 QString const& label, QString const& y_unit_string)
{
    channels_[id] = data_block;
    channel_labels_[id] = label;
    y_unit_strings_[id] = y_unit_string;
}

//-------------------------------------------------------------------------
ChannelID ProcessedSignalChannelManager::addExtraChannel (ChannelID id, QSharedPointer<DataBlock const> data_block,
                                                     QString const& label, QString const& y_unit_string)
{
    id += 0.1f;
    channels_[id] = data_block;
    channel_labels_[id] = label;
    y_unit_strings_[id] = y_unit_string;
    return id;
}

//-------------------------------------------------------------------------
std::set<ChannelID> ProcessedSignalChannelManager::getChannels () const
{
    std::set<ChannelID> channels;
    foreach (ChannelID id, channels_.keys())
        channels.insert(id);
    return channels;
}


//-------------------------------------------------------------------------
uint32 ProcessedSignalChannelManager::getNumberChannels () const
{
    return channels_.size ();
}

//-------------------------------------------------------------------------
QString ProcessedSignalChannelManager::getChannelLabel (ChannelID id) const
{
    return channel_labels_[id];
}

//-------------------------------------------------------------------------
QString ProcessedSignalChannelManager::getChannelYUnitString (ChannelID id) const
{
    return y_unit_strings_[id];
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock const> ProcessedSignalChannelManager::getData (ChannelID id,
                                                 unsigned start_pos,
                                                 unsigned length) const
{
    return channels_[id]->createSubBlock (start_pos, length);
}

//-------------------------------------------------------------------------
float64 ProcessedSignalChannelManager::getDurationInSec() const
{
    return static_cast<float32>(length_) / sample_rate_;
}

//-------------------------------------------------------------------------
size_t ProcessedSignalChannelManager::getNumberSamples() const
{
    return length_;
}

//-------------------------------------------------------------------------
float64 ProcessedSignalChannelManager::getSampleRate() const
{
    return sample_rate_;
}

}
