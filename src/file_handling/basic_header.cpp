// basic_header.cpp

#include "basic_header.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
QString BasicHeader::getFileTypeString () const
{
    return file_type_string_;
}

//-----------------------------------------------------------------------------
float32 BasicHeader::getSampleRate () const
{
    return sample_rate_;
}

//-----------------------------------------------------------------------------
int BasicHeader::addFilter (QSharedPointer<Filter> filter, int before_index)
{
    if ((before_index < 0) || (before_index >= filters_.size ()))
    {
        filters_.append (filter);
        return filters_.size() - 1;
    }
    else
    {
        filters_.insert (before_index, filter);
        return before_index;
    }
}


//-----------------------------------------------------------------------------
uint32 BasicHeader::getNumberEvents() const
{
    return number_events_;
}

void BasicHeader::setNumberEvents (uint32 number_events)
{
    number_events_ = number_events;
}

// get event samplerate
double BasicHeader::getEventSamplerate() const
{
    return sample_rate_;
}

void BasicHeader::setEventSamplerate (double event_sample_rate)
{
    event_sample_rate_ = event_sample_rate;
}

//-------------------------------------------------------------------------
QSharedPointer<SignalChannel const> BasicHeader::getChannel (ChannelID id) const
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
void BasicHeader::setFileTypeString (QString const& file_type_string)
{
    file_type_string_ = file_type_string;
}

//-------------------------------------------------------------------------
void BasicHeader::setSampleRate (float sample_rate, int downsampling_factor)
{
    sample_rate_ = sample_rate;
    downsampling_factor_ = downsampling_factor;
}

//-------------------------------------------------------------------------
void BasicHeader::addChannel (ChannelID id, QSharedPointer<SignalChannel const> channel)
{
    channels_[id] = channel;
}

} // namespace SigViewer_
