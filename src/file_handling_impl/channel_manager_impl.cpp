#include "channel_manager_impl.h"

#include <cmath>

namespace BioSig_
{

//-----------------------------------------------------------------------------
ChannelManagerImpl::ChannelManagerImpl (FileSignalReader& file_signal_reader)
    : reader_ (file_signal_reader)
{

}

//-----------------------------------------------------------------------------
ChannelManagerImpl::~ChannelManagerImpl ()
{

}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> ChannelManagerImpl::getData (ChannelID id, unsigned start_pos, unsigned length) const
{
    return reader_.getSignalData(id, start_pos, length);
//    std::vector<float32> data (length);
//    for (unsigned sample_index = start_pos; sample_index < start_pos + length; sample_index++)
//        data[sample_index - start_pos] = sin (static_cast<float32>(sample_index) / 100.0f) * 20.0f;
//
//    DataBlock data_block (data, 50);
//
//    return data_block;
}

//-----------------------------------------------------------------------------
float32 ChannelManagerImpl::getDurationInSec () const
{
    return reader_.getBasicHeader()->getNumberOfSamples() /
           reader_.getBasicHeader()->getSampleRate();
}


//-----------------------------------------------------------------------------
uint32 ChannelManagerImpl::getNumberSamples () const
{
    return reader_.getBasicHeader()->getNumberOfSamples();
}

//-----------------------------------------------------------------------------
float32 ChannelManagerImpl::getSampleRate () const
{
    return reader_.getBasicHeader()->getSampleRate();
}

//-----------------------------------------------------------------------------
float64 ChannelManagerImpl::getMinValue (ChannelID channel_id) const
{
    QSharedPointer<DataBlock const> channel_data = reader_.getSignalData (channel_id, 0,
                                                    reader_.getBasicHeader()->getNumberOfSamples());

    return channel_data->getMin();
}

//-----------------------------------------------------------------------------
float64 ChannelManagerImpl::getMaxValue (ChannelID channel_id) const
{
    QSharedPointer<DataBlock const> channel_data = reader_.getSignalData (channel_id, 0,
                                                    reader_.getBasicHeader()->getNumberOfSamples());

    return channel_data->getMax();
}



}
