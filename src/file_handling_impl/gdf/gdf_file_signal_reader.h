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
