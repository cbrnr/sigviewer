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
    for (ChannelID id = 0; id < reader_->getBasicHeader()->getNumberChannels(); ++id)
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
    else if (reader_->getBasicHeader()->getNumberChannels() > id)
        return reader_->getBasicHeader()->getChannel (id).getLabel();
    else
        return QObject::tr("Invalid Channel");
}


//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> ChannelManagerImpl::getData (ChannelID id, unsigned start_pos, unsigned length) const
{
    return reader_->getSignalData(id, start_pos, length);
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
