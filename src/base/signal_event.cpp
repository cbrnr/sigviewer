// signal_event.cpp

#include "signal_event.h"

namespace BioSig_
{

// constructor
SignalEvent::SignalEvent()
: id_(UNDEFINED_ID),
  position_(0xFFFFFFFF),
  type_(0xFFFF),
  channel_(UNDEFINED_CHANNEL),
  duration_(UNDEFINED_DURATION)
{
    // nothing
}

// constructor
SignalEvent::SignalEvent(uint32 position, uint16 type, int16 channel,
                         uint32 duration, int32 id)
: id_(id),
  position_(position),
  type_(type),
  channel_(channel),
  duration_(duration)
{
    // nothing
}

// constructor
SignalEvent::SignalEvent(const SignalEvent& src, int32 id)
: id_(id),
  position_(src.position_),
  type_(src.type_),
  channel_(src.channel_),
  duration_(src.duration_)
{
    // nothing   
}

// get id
int32 SignalEvent::getId() const
{
    return id_;
}

// get position
uint32 SignalEvent::getPosition() const
{
    return position_;
}

// get type
uint16 SignalEvent::getType() const
{
    return type_;
}

// get channel
int16 SignalEvent::getChannel() const
{
    return channel_;
}

// get duration
uint32 SignalEvent::getDuration() const
{
    return duration_;
}

//-----------------------------------------------------------------------------
void SignalEvent::setId (int32 id)
{
    id_ = id;
}

// set position
void SignalEvent::setPosition(uint32 position)
{
    position_ = position;
}

// set type
void SignalEvent::setType(uint16 type)
{
    type_ = type;
}

// set channel
void SignalEvent::setChannel(int16 channel)
{
    channel_ = channel;
}

// set duration
void SignalEvent::setDuration(uint32 duration)
{
    duration_ = duration;
}

} // namespace BioSig_
