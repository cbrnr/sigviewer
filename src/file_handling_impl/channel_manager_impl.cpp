#include "channel_manager_impl.h"

#include <cmath>

namespace BioSig_
{

//-----------------------------------------------------------------------------
ChannelManagerImpl::ChannelManagerImpl (QSharedPointer<FileSignalReader> file_signal_reader)
    : reader_ (file_signal_reader)
{

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
    else if (static_cast<int>(reader_->getBasicHeader()->getNumberChannels()) > id)
        return reader_->getBasicHeader()->getChannel (id).getLabel();
    else
        return QObject::tr("Invalid Channel");
}

//-------------------------------------------------------------------------
QString ChannelManagerImpl::getChannelYUnitString (ChannelID id) const
{
    if (id != UNDEFINED_CHANNEL)
        return reader_->getBasicHeader()->getChannel(id).getPhysicalDim();
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

//-------------------------------------------------------------------------
boost::tuples::tuple<double, double> ChannelManagerImpl::getPhysMinMax (ChannelID channel_id) const
{
    double phys_min = reader_->getBasicHeader()->getChannel(channel_id).getPhysicalMinimum();
    double phys_max = reader_->getBasicHeader()->getChannel(channel_id).getPhysicalMaximum();
    return boost::tuples::tuple<double, double> (phys_min, phys_max);
}

//-----------------------------------------------------------------------------
float32 ChannelManagerImpl::getDurationInSec () const
{
    return reader_->getBasicHeader()->getNumberOfSamples() /
           reader_->getBasicHeader()->getSampleRate();
}


//-----------------------------------------------------------------------------
uint32 ChannelManagerImpl::getNumberSamples () const
{
    return reader_->getBasicHeader()->getNumberOfSamples();
}

//-----------------------------------------------------------------------------
float32 ChannelManagerImpl::getSampleRate () const
{
    return reader_->getBasicHeader()->getSampleRate();
}

}
