// signal_event.h

#ifndef SIGNAL_EVENT_H
#define SIGNAL_EVENT_H

#include "user_types.h"

namespace BioSig_
{

// SignalEvent
class SignalEvent
{
public:
    static const int32 UNDEFINED_DURATION = 0;


    enum
    {
        // add 0x8000 to type to indicate end of event
        EVENT_END = 0x8000,
    };


    SignalEvent();
    SignalEvent(uint32 position, EventType type,
                float64 sample_rate,
                ChannelID channel = UNDEFINED_CHANNEL,
                uint32 duration = UNDEFINED_DURATION, EventID id = UNDEFINED_EVENT_ID);

    SignalEvent(const SignalEvent& src, int32 id);
    SignalEvent(const SignalEvent& src);
    SignalEvent& operator= (const SignalEvent& src);

    int32 getId() const;
    uint32 getPosition() const;
    float32 getPositionInSec() const;
    uint16 getType() const;
    ChannelID getChannel() const;
    uint32 getDuration() const;
    float32 getDurationInSec() const;
    float32 getEndInSec () const;
    float64 getSampleRate () const;

    void setId (EventID id);
    void setPosition(uint32 position);
    void setType(EventType type);
    void setChannel(ChannelID channel);
    void setDuration(uint32 duration);

private:
    EventID id_;
    uint32 position_;
    float64 sample_rate_;
    EventType type_;
    ChannelID channel_;
    uint32 duration_;
};

} // namespace BioSig_

#endif
