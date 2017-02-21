// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef PROCESSED_SIGNAL_CHANNEL_MANAGER_H
#define PROCESSED_SIGNAL_CHANNEL_MANAGER_H

#include "file_handling/channel_manager.h"

#include <QMap>
#include <QObject>

namespace sigviewer
{

class ProcessedSignalChannelManager : public ChannelManager, public QObject
{
public:
    //-------------------------------------------------------------------------
    ProcessedSignalChannelManager(float64 sample_rate, unsigned length, QObject* parent);

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
    virtual float64 getDurationInSec() const;

    //-------------------------------------------------------------------------
    virtual size_t getNumberSamples() const;

    //-------------------------------------------------------------------------
    virtual float64 getSampleRate() const;

private:
    virtual QString getChannelLabel (ChannelID id, int streamNumber) const; /*!< Inherited, should not be called. */

    float32 sample_rate_;
    unsigned length_;
    QMap<ChannelID, QSharedPointer<DataBlock const> > channels_;
    QMap<ChannelID, QString> channel_labels_;
    QMap<ChannelID, QString> y_unit_strings_;
};

}

#endif // PROCESSED_SIGNAL_CHANNEL_MANAGER_H
