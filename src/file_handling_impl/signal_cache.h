#ifndef SIGNAL_CACHE_H
#define SIGNAL_CACHE_H

#include "base/sigviewer_user_types.h"

namespace SigViewer_
{

class SignalCache
{
public:
    virtual ~SignalCache () {}
    virtual float32 getSample (ChannelID channel, unsigned sample_index) = 0;
};

}

#endif // SIGNAL_CACHE_H
