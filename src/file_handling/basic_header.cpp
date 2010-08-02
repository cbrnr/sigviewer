// basic_header.cpp

#include "basic_header.h"

#include <QMutexLocker>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
QString BasicHeader::getFileTypeString () const
{
    return file_type_string_;
}


// get recording time
const QDateTime& BasicHeader::getRecordingTime() const
{
    return recording_time_;
}

// get reference
const QString& BasicHeader::getReference() const
{
    return reference_;
}

// triggered
bool BasicHeader::isTriggered() const
{
    return triggered_;
}

// get number of records
int64 BasicHeader::getNumberRecords() const
{
    return number_records_;
}

void BasicHeader::setNumberRecords (int64 number_records)
{
    number_records_ = number_records;
}

// get record duration
float64 BasicHeader::getRecordDuration() const
{
    return record_duration_;
}

void BasicHeader::setRecordDuration (float64 record_duration)
{
    record_duration_ = record_duration;
}

// get file-size
uint32 BasicHeader::getFileSize() const
{
    return file_size_;
}

void BasicHeader::setFileSize(uint32 file_size)
{
    file_size_ = file_size;
}

//-----------------------------------------------------------------------------
float32 BasicHeader::getSampleRate () const
{
    return sample_rate_;
}

// get number of events
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
    return event_sample_rate_;
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
void BasicHeader::setSampleRate (float sample_rate)
{
    sample_rate_ = sample_rate;
}

//-------------------------------------------------------------------------
void BasicHeader::addChannel (ChannelID id, QSharedPointer<SignalChannel const> channel)
{
    channels_[id] = channel;
}

} // namespace SigViewer_
