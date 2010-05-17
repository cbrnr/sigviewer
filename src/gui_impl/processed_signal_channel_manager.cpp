#include "processed_signal_channel_manager.h"

namespace BioSig_
{

//-------------------------------------------------------------------------
ProcessedSignalChannelManager::ProcessedSignalChannelManager (float32 sample_rate, unsigned length)
    : sample_rate_ (sample_rate),
      length_ (length)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
void ProcessedSignalChannelManager::addChannel (ChannelID id, QSharedPointer<DataBlock const> data_block,
                 QString const& label)
{
    channels_[id] = data_block;
    channel_labels_[id] = label;
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
QSharedPointer<DataBlock const> ProcessedSignalChannelManager::getData (ChannelID id,
                                                 unsigned start_pos,
                                                 unsigned length) const
{
    return channels_[id]->createSubBlock (start_pos, length);
}

//-------------------------------------------------------------------------
float32 ProcessedSignalChannelManager::getDurationInSec () const
{
    return static_cast<float32>(length_) / sample_rate_;
}

//-------------------------------------------------------------------------
uint32 ProcessedSignalChannelManager::getNumberSamples () const
{
    return length_;
}

//-------------------------------------------------------------------------
float32 ProcessedSignalChannelManager::getSampleRate () const
{
    return sample_rate_;
}

}

