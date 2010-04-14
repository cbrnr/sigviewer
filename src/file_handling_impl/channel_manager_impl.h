#ifndef CHANNEL_MANAGER_IMPL_H
#define CHANNEL_MANAGER_IMPL_H

#include "../file_handling/channel_manager.h"
#include "../file_handling/file_signal_reader.h"

#include <QSharedPointer>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// ChannelManagerImpl
///
/// implementation of ChannelManager
///
class ChannelManagerImpl : public ChannelManager
{
public:
    ChannelManagerImpl (QSharedPointer<FileSignalReader> file_signal_reader);
    virtual ~ChannelManagerImpl ();


    //-------------------------------------------------------------------------
    virtual uint32 getNumberChannels () const;

    //-------------------------------------------------------------------------
    virtual SignalChannel const& getSignalChannel (ChannelID id) const;

    //-------------------------------------------------------------------------
    virtual QString const& getChannelLabel (ChannelID id) const;


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

    //-------------------------------------------------------------------------
    virtual float64 getMinValue (ChannelID channel_id) const;

    //-------------------------------------------------------------------------
    virtual float64 getMaxValue (ChannelID channel_id) const;


private:
    QSharedPointer<FileSignalReader> reader_;
};

}

#endif // CHANNEL_MANAGER_IMPL_H
