// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


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
                float64 sample_rate,
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
};

} // namespace SigViewer_

#endif
