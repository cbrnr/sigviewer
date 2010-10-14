#ifndef PROCESSED_SIGNAL_CHANNEL_MANAGER_H
#define PROCESSED_SIGNAL_CHANNEL_MANAGER_H

#include "file_handling/channel_manager.h"

#include <QMap>
#include <QObject>

namespace SigViewer_
{

class ProcessedSignalChannelManager : public ChannelManager, public QObject
{
public:
    //-------------------------------------------------------------------------
    ProcessedSignalChannelManager (float32 sample_rate, unsigned length, QObject* parent);

    //-------------------------------------------------------------------------
    void addChannel (ChannelID id, QSharedPointer<DataBlock const> data_block,
                     QString const& label, QString const& y_unit_string);

    //-------------------------------------------------------------------------
    ChannelID addExtraChannel (ChannelID id, QSharedPointer<DataBlock const> data_block,
                          QString const& label, QString const& y_unit_string);

    //-------------------------------------------------------------------------
    virtual ~ProcessedSignalChannelManager () {}

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
    float32 sample_rate_;
    unsigned length_;
    QMap<ChannelID, QSharedPointer<DataBlock const> > channels_;
    QMap<ChannelID, QString> channel_labels_;
    QMap<ChannelID, QString> y_unit_strings_;
};

}

#endif // PROCESSED_SIGNAL_CHANNEL_MANAGER_H
