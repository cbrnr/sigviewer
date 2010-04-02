#ifndef CHANNEL_MANAGER_INTERFACE_H
#define CHANNEL_MANAGER_INTERFACE_H

#include "../base/user_types.h"
#include "../base/data_block.h"

namespace BioSig_
{


const ChannelID UNDEFINDED_CHANNEL = -1;

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
    virtual DataBlock getData (ChannelID id, unsigned start_pos, unsigned length) const;

};

} // namespace BioSig_

#endif // CHANNEL_MANAGER_INTERFACE_H
