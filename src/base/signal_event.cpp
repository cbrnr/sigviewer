// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "signal_event.h"

namespace sigviewer
{

// constructor
SignalEvent::SignalEvent()
: id_(UNDEFINED_EVENT_ID),
  position_(0xFFFFFFFF),
  sample_rate_ (1),
  type_(0xFFFF),
  channel_(UNDEFINED_CHANNEL),
  duration_(UNDEFINED_DURATION),
  stream_(-1)
{
    // nothing
}


// constructor
SignalEvent::SignalEvent(size_t position, EventType type, float64 sample_rate, int streamNumber, ChannelID channel,
                         size_t duration, int32 id)
: id_(id),
  position_(position),
  sample_rate_ (sample_rate),
  type_(type),
  channel_(channel),
  duration_(duration),
  stream_(streamNumber)
{
    // nothing
}

// constructor
SignalEvent::SignalEvent(const SignalEvent& src, int32 id)
: id_(id),
  position_(src.position_),
  sample_rate_ (src.sample_rate_),
  type_(src.type_),
  channel_(src.channel_),
  duration_(src.duration_),
  stream_(src.stream_)
{
    // nothing   
}

// copy-constructor
SignalEvent::SignalEvent(const SignalEvent& src)
: id_(src.id_),
  position_(src.position_),
  sample_rate_ (src.sample_rate_),
  type_(src.type_),
  channel_(src.channel_),
  duration_(src.duration_),
  stream_(src.stream_)
{
    // nothing
}

SignalEvent& SignalEvent::operator= (const SignalEvent& src)
{
    id_ = src.id_;
    position_ = src.position_;
    sample_rate_ = src.sample_rate_;
    type_ = src.type_;
    channel_ = src.channel_;
    duration_ = src.duration_;
    stream_ = src.stream_;
    return *this;
}


// get id
int32 SignalEvent::getId() const
{
    return id_;
}

// get position
size_t SignalEvent::getPosition() const
{
    return position_;
}

//-----------------------------------------------------------------------------
float64 SignalEvent::getPositionInSec() const
{
    return static_cast<float64>(position_) / sample_rate_;
}


// get type
uint16 SignalEvent::getType() const
{
    return type_;
}

// get channel
ChannelID SignalEvent::getChannel() const
{
    return channel_;
}

// get duration
size_t SignalEvent::getDuration() const
{
    return duration_;
}

//-----------------------------------------------------------------------------
float64 SignalEvent::getDurationInSec() const
{
    return static_cast<float64>(duration_) / sample_rate_;
}

//-----------------------------------------------------------------------------
float64 SignalEvent::getEndInSec () const
{
    return (static_cast<float64>(duration_ + position_)) / sample_rate_;
}

//-----------------------------------------------------------------------------
float64 SignalEvent::getSampleRate () const
{
    return sample_rate_;
}

int SignalEvent::getStream() const
{
    return stream_;
}

//-----------------------------------------------------------------------------
void SignalEvent::setId (EventID id)
{
    id_ = id;
}

// set position
void SignalEvent::setPosition(size_t position)
{
    position_ = position;
}

// set type
void SignalEvent::setType(EventType type)
{
    type_ = type;
}

// set channel
void SignalEvent::setChannel(ChannelID channel)
{
    channel_ = channel;
}

// set duration
void SignalEvent::setDuration(size_t duration)
{
    duration_ = duration;
}

//-----------------------------------------------------------------------------
bool SignalEvent::equals (SignalEvent const& event) const
{
    if (duration_ != event.duration_)
        return false;
    if (type_ != event.type_)
        return false;
    if (position_ != event.position_)
        return false;
    if (sample_rate_ != event.sample_rate_)
        return false;
    if (channel_ != event.channel_)
        return false;
    if (stream_ != event.stream_)
        return false;

    return true;
}

}
