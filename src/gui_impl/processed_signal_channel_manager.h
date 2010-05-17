#ifndef PROCESSED_SIGNAL_CHANNEL_MANAGER_H
#define PROCESSED_SIGNAL_CHANNEL_MANAGER_H

#include "../file_handling/channel_manager.h"

#include <QMap>

namespace BioSig_
{

class ProcessedSignalChannelManager : public ChannelManager
{
public:
    //-------------------------------------------------------------------------
    ProcessedSignalChannelManager (float32 sample_rate, unsigned length);

    //-------------------------------------------------------------------------
    void addChannel (ChannelID id, QSharedPointer<DataBlock const> data_block,
                     QString const& label);

    //-------------------------------------------------------------------------
    virtual ~ProcessedSignalChannelManager () {}

    //-------------------------------------------------------------------------
    virtual std::set<ChannelID> getChannels () const;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberChannels () const;

    //-------------------------------------------------------------------------
    virtual QString getChannelLabel (ChannelID id) const;


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
    float32 sample_rate_;
    unsigned length_;
    QMap<ChannelID, QSharedPointer<DataBlock const> > channels_;
    QMap<ChannelID, QString> channel_labels_;
};

}

#endif // PROCESSED_SIGNAL_CHANNEL_MANAGER_H
