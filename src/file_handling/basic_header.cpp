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


// basic_header.cpp

#include "basic_header.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
QString BasicHeader::getFileTypeString() const
{
    return file_type_string_;
}

//-----------------------------------------------------------------------------
float64 BasicHeader::getSampleRate() const
{
    return sample_rate_;
}

//-----------------------------------------------------------------------------
uint32 BasicHeader::getNumberEvents() const
{
    return number_events_;
}

void BasicHeader::setNumberEvents(uint32 number_events)
{
    number_events_ = number_events;
}

// get event samplerate
double BasicHeader::getEventSamplerate() const
{
    return sample_rate_;
}

void BasicHeader::setEventSamplerate(double event_sample_rate)
{
    event_sample_rate_ = event_sample_rate;
}

//-------------------------------------------------------------------------
QSharedPointer<SignalChannel const> BasicHeader::getChannel(ChannelID id) const
{
    if (channels_.contains(id))
        return channels_[id];
    return QSharedPointer<SignalChannel const> (0);
}


//-------------------------------------------------------------------------
unsigned BasicHeader::getNumberChannels() const
{
    return channels_.size();
}

//-------------------------------------------------------------------------
void BasicHeader::setFileTypeString(QString const& file_type_string)
{
    file_type_string_ = file_type_string;
}

//-------------------------------------------------------------------------
void BasicHeader::setSampleRate(float64 sample_rate)
{
    sample_rate_ = sample_rate;
}

//-------------------------------------------------------------------------
void BasicHeader::addChannel(ChannelID id, QSharedPointer<SignalChannel const> channel)
{
    channels_[id] = channel;
}

} // namespace SigViewer_
