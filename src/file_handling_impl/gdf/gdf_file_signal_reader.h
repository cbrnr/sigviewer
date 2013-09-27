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


#ifndef GDF_FILE_SIGNAL_READER_H
#define GDF_FILE_SIGNAL_READER_H

#include "file_handling/file_signal_reader.h"
#include "gdf_basic_header.h"
#include "gdf_signal_cache.h"
#include "gdf_data_block.h"
#include "../down_sampling_thread.h"

#include "GDF/Reader.h"

#include <QObject>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class GDFFileSignalReader : public QObject, public FileSignalReader
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    GDFFileSignalReader ();

    //-------------------------------------------------------------------------
    virtual ~GDFFileSignalReader();

    //-------------------------------------------------------------------------
    QPair<FileSignalReader*, QString> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID channel_id,
                                                           unsigned start_sample,
                                                           unsigned length) const;

    //-------------------------------------------------------------------------
    virtual QList<QSharedPointer<SignalEvent const> > getEvents () const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader> getBasicHeader ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader const> getBasicHeader () const {return header_;}

private:
    //-------------------------------------------------------------------------
    QString open (QString const& file_path);

    Q_DISABLE_COPY(GDFFileSignalReader)


    gdf::Reader* reader_;
    mutable DownSamplingThread* downsampling_thread_;

    QSharedPointer<GDFSignalCache> cache_;
    QSharedPointer<GDFBasicHeader> header_;
    mutable QMap<ChannelID, QSharedPointer<GDFDataBlock> > channel_map_;
    mutable QList<QSharedPointer<SignalEvent const> > events_;
};

} // namespace

#endif // GDF_FILE_SIGNAL_READER_H
