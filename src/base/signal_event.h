// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef SIGNAL_EVENT_H
#define SIGNAL_EVENT_H

#include "sigviewer_user_types.h"

namespace sigviewer
{

// SignalEvent
class SignalEvent
{
public:
    SignalEvent();
    SignalEvent(size_t position, EventType type,
                float64 sample_rate, int streamNumber,
                ChannelID channel = UNDEFINED_CHANNEL,
                size_t duration = 0, EventID id = UNDEFINED_EVENT_ID);

    SignalEvent(const SignalEvent& src, int32 id);
    SignalEvent(const SignalEvent& src);
    SignalEvent& operator= (const SignalEvent& src);

    int32 getId() const;
    size_t getPosition() const;
    float64 getPositionInSec() const;
    uint16 getType() const;
    ChannelID getChannel() const;
    size_t getDuration() const;
    float64 getDurationInSec() const;
    float64 getEndInSec () const;
    float64 getSampleRate () const;
    int getStream() const;

    void setId (EventID id);
    void setPosition(size_t position);
    void setType(EventType type);
    void setChannel(ChannelID channel);
    void setDuration(size_t duration);

    bool equals (SignalEvent const& event) const;

private:
    static const int32 UNDEFINED_DURATION = 0;

    EventID id_;
    size_t position_;
    float64 sample_rate_;
    EventType type_;
    ChannelID channel_;
    size_t duration_;
    int stream_;
};

}

#endif
