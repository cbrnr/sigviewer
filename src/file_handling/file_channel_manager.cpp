// © SigViewer developers
//
// License: GPL-3.0


#include "file_channel_manager.h"


namespace sigviewer
{

//-----------------------------------------------------------------------------
FileChannelManager::FileChannelManager (FileSignalReader* file_signal_reader)
    : reader_ (file_signal_reader)
{
    setXAxisUnitLabel ("s");
}


//-----------------------------------------------------------------------------
FileChannelManager::~FileChannelManager ()
{
    delete reader_;
}

//-------------------------------------------------------------------------
std::set<ChannelID> FileChannelManager::getChannels () const
{
    std::set<ChannelID> channels;
    for (ChannelID id = 0; id < static_cast<int>(reader_->getBasicHeader()->getNumberChannels()); ++id)
        channels.insert(id);
    return channels;
}

//-----------------------------------------------------------------------------
uint32 FileChannelManager::getNumberChannels () const
{
    return reader_->getBasicHeader()->getNumberChannels();
}

//-------------------------------------------------------------------------
QString FileChannelManager::getChannelLabel (ChannelID id) const
{
    if (id == UNDEFINED_CHANNEL)
        return QObject::tr("All Channels");
    else if (!reader_->getBasicHeader()->getChannel (id).isNull())
        return reader_->getBasicHeader()->getChannel (id)->getLabel();
    else
        return QObject::tr("Invalid Channel");
}

//-------------------------------------------------------------------------
QString FileChannelManager::getChannelLabel (ChannelID id, int streamNumber) const
{
    if (id == UNDEFINED_CHANNEL)
    {
        if (streamNumber == -1)
            return QObject::tr("All Channels");
        else
            return QObject::tr("Stream %1").arg(QString::number(streamNumber+1));//+1 for user's convenience (1 based instead 0 based)
    }
    else if (!reader_->getBasicHeader()->getChannel (id).isNull())
        return reader_->getBasicHeader()->getChannel (id)->getLabel();
    else
        return QObject::tr("Invalid Channel");
}

//-------------------------------------------------------------------------
QString FileChannelManager::getChannelYUnitString (ChannelID id) const
{
    if (id != UNDEFINED_CHANNEL)
        return reader_->getBasicHeader()->getChannel(id)->getPhysicalDim();
    else
        return "";
   // reader_->getBasicHeader()->get
}


//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> FileChannelManager::getData (ChannelID id, unsigned start_pos, unsigned length) const
{
    if (((start_pos + length) > getNumberSamples()) || length == 0)
        return QSharedPointer<DataBlock const> (0);
    else
        return reader_->getSignalData (id, start_pos, length);
}

//-----------------------------------------------------------------------------
float64 FileChannelManager::getDurationInSec () const
{
    return reader_->getBasicHeader()->getNumberOfSamples() /
           reader_->getBasicHeader()->getSampleRate();
}


//-----------------------------------------------------------------------------
size_t FileChannelManager::getNumberSamples () const
{
    return reader_->getBasicHeader()->getNumberOfSamples();
}

//-----------------------------------------------------------------------------
float64 FileChannelManager::getSampleRate () const
{
    return reader_->getBasicHeader()->getSampleRate();
}

}
