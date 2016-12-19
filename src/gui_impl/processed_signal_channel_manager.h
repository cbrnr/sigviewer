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
    float32 sample_rate_;
    unsigned length_;
    QMap<ChannelID, QSharedPointer<DataBlock const> > channels_;
    QMap<ChannelID, QString> channel_labels_;
    QMap<ChannelID, QString> y_unit_strings_;
};

}

#endif // PROCESSED_SIGNAL_CHANNEL_MANAGER_H
