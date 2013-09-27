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


#include "channel_manager_impl.h"

#include <cmath>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
ChannelManagerImpl::ChannelManagerImpl (FileSignalReader* file_signal_reader)
    : reader_ (file_signal_reader)
{
    setXAxisUnitLabel ("s");
}


//-----------------------------------------------------------------------------
ChannelManagerImpl::~ChannelManagerImpl ()
{
    delete reader_;
}

//-------------------------------------------------------------------------
std::set<ChannelID> ChannelManagerImpl::getChannels () const
{
    std::set<ChannelID> channels;
    for (ChannelID id = 0; id < static_cast<int>(reader_->getBasicHeader()->getNumberChannels()); ++id)
        channels.insert(id);
    return channels;
}

//-----------------------------------------------------------------------------
uint32 ChannelManagerImpl::getNumberChannels () const
{
    return reader_->getBasicHeader()->getNumberChannels();
}

//-------------------------------------------------------------------------
QString ChannelManagerImpl::getChannelLabel (ChannelID id) const
{
    if (id == UNDEFINED_CHANNEL)
        return QObject::tr("All Channels");
    else if (!reader_->getBasicHeader()->getChannel (id).isNull())
        return reader_->getBasicHeader()->getChannel (id)->getLabel();
    else
        return QObject::tr("Invalid Channel");
}

//-------------------------------------------------------------------------
QString ChannelManagerImpl::getChannelYUnitString (ChannelID id) const
{
    if (id != UNDEFINED_CHANNEL)
        return reader_->getBasicHeader()->getChannel(id)->getPhysicalDim();
    else
        return "";
   // reader_->getBasicHeader()->get
}


//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> ChannelManagerImpl::getData (ChannelID id, unsigned start_pos, unsigned length) const
{
    if (((start_pos + length) > getNumberSamples()) || length == 0)
        return QSharedPointer<DataBlock const> (0);
    else
        return reader_->getSignalData (id, start_pos, length);
}

//-----------------------------------------------------------------------------
float64 ChannelManagerImpl::getDurationInSec () const
{
    return reader_->getBasicHeader()->getNumberOfSamples() /
           reader_->getBasicHeader()->getSampleRate();
}


//-----------------------------------------------------------------------------
size_t ChannelManagerImpl::getNumberSamples () const
{
    return reader_->getBasicHeader()->getNumberOfSamples();
}

//-----------------------------------------------------------------------------
float64 ChannelManagerImpl::getSampleRate () const
{
    return reader_->getBasicHeader()->getSampleRate();
}

}
