#ifndef CHANNEL_MANAGER_INTERFACE_H
#define CHANNEL_MANAGER_INTERFACE_H

#include "../base/user_types.h"
#include "../base/data_block.h"

#include <QSharedPointer>

namespace BioSig_
{



//-----------------------------------------------------------------------------
/// ChannelManager
///
/// abstract base class for any channel handling
class ChannelManager
{
public:
    //-------------------------------------------------------------------------
    /// destructor
    virtual ~ChannelManager () {}

    //-------------------------------------------------------------------------
    /// @param channel_id the id of the channel
    /// @param start_pos starting sample
    /// @param length length given in samples
    virtual QSharedPointer<DataBlock const> getData (ChannelID id,
                                                     unsigned start_pos,
                                                     unsigned length) const = 0;

    //-------------------------------------------------------------------------
    virtual float32 getDurationInSec () const = 0;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberSamples () const = 0;

    //-------------------------------------------------------------------------
    virtual float32 getSampleRate () const = 0;

    //-------------------------------------------------------------------------
    virtual float64 getMinValue (ChannelID channel_id) const = 0;

    //-------------------------------------------------------------------------
    virtual float64 getMaxValue (ChannelID channel_id) const = 0;
};

} // namespace BioSig_

#endif // CHANNEL_MANAGER_INTERFACE_H
