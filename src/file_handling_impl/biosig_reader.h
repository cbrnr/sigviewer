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


#ifndef BIOSIG_READER_H_
#define BIOSIG_READER_H_

#include "file_handling/file_signal_reader.h"
#include "biosig.h"

#include <QFile>
#include <QMutex>
#include <QMap>

namespace SigViewer_
{

class BioSigReader : public FileSignalReader
{
public:
    BioSigReader ();

    virtual ~BioSigReader();

    //-------------------------------------------------------------------------
    QPair<FileSignalReader*, QString> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID channel_id,
                                                           size_t start_sample,
                                                           size_t length) const;

    //-------------------------------------------------------------------------
    virtual QList<QSharedPointer<SignalEvent const> > getEvents () const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader> getBasicHeader ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader const> getBasicHeader () const {return basic_header_;}

private:
    //-------------------------------------------------------------------------
    QString open (QString const& file_name);

    //-------------------------------------------------------------------------
    void bufferAllChannels () const;

    //-------------------------------------------------------------------------
    void applyFilters (double* &in, double* &out, int length) const;

    //-------------------------------------------------------------------------
    void bufferAllEvents () const;

    Q_DISABLE_COPY(BioSigReader)

    QString loadFixedHeader(const QString& file_name);

    void doClose () const;

    QSharedPointer<BasicHeader> basic_header_;
    mutable QMutex mutex_;
    mutable QMutex biosig_access_lock_;
    mutable HDRTYPE* biosig_header_;
    mutable bool buffered_all_channels_;
    mutable bool buffered_all_events_;
    mutable QMap<ChannelID, QSharedPointer<DataBlock const> > channel_map_;
    mutable QList<QSharedPointer<SignalEvent const> > events_;
};

} // namespace SigViewer_

#endif /*CNT_READER_H_*/
