#ifndef CHANNEL_MANAGER_IMPL_H
#define CHANNEL_MANAGER_IMPL_H

#include "file_handling/channel_manager.h"
#include "file_handling/file_signal_reader.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// ChannelManagerImpl
///
/// implementation of ChannelManager
///
class ChannelManagerImpl : public ChannelManager
{
public:
    //---------------------------------------------------------------------------------------------
    /// @param file_signal_reader the ChannelManagerImpl takes ownership of the given FileSignalReader
    ///                           and destroys it if the ChannelManagerImpl is destructed
    ChannelManagerImpl (FileSignalReader* file_signal_reader);

    //---------------------------------------------------------------------------------------------
    virtual ~ChannelManagerImpl ();

    //-------------------------------------------------------------------------
    virtual std::set<ChannelID> getChannels () const;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberChannels () const;

    //-------------------------------------------------------------------------
    virtual QString getChannelLabel (ChannelID id) const;

    //-------------------------------------------------------------------------
    virtual QString getChannelYUnitString (ChannelID id) const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock const> getData (ChannelID id,
                                                     unsigned start_pos,
                                                     unsigned length) const;

    //-------------------------------------------------------------------------
    virtual float32 getDurationInSec () const;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberSamples () const;

    //-------------------------------------------------------------------------
    virtual float32 getSampleRate () const;

private:
    FileSignalReader* reader_;
};

}

#endif // CHANNEL_MANAGER_IMPL_H
