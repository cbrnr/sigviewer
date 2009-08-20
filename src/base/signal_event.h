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
    enum
    {
        UNDEFINED_CHANNEL = -1
    };

    enum
    {
        UNDEFINED_DURATION = 0
    };

    enum
    {
        // add 0x8000 to type to indicate end of event
        EVENT_END = 0x8000,
    };

    enum
    {
        UNDEFINED_ID = -1   
    };


    SignalEvent();
    SignalEvent(uint32 position, uint16 type,
                int16 channel = UNDEFINED_CHANNEL,
                uint32 duration = UNDEFINED_DURATION, int32 id = UNDEFINED_ID);

    SignalEvent(const SignalEvent& src, int32 id = UNDEFINED_ID);

    int32 getId() const;
    uint32 getPosition() const;
    uint16 getType() const;
    int16 getChannel() const;
    uint32 getDuration() const;

    void setId (int32 id);
    void setPosition(uint32 position);
    void setType(uint16 type);
    void setChannel(int16 channel);
    void setDuration(uint32 duration);

private:
    int32 id_;
    uint32 position_;
    uint16 type_;
    int16 channel_;
    uint32 duration_;
};

} // namespace BioSig_

#endif
