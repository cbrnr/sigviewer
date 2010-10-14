#ifndef GDF_FILE_SIGNAL_READER_H
#define GDF_FILE_SIGNAL_READER_H

#include "file_handling/file_signal_reader.h"
#include "gdf_basic_header.h"

#include "GDF/Reader.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class GDFFileSignalReader : public FileSignalReader
{
public:
    //-------------------------------------------------------------------------
    GDFFileSignalReader ();

    //-------------------------------------------------------------------------
    virtual ~GDFFileSignalReader();

    //-------------------------------------------------------------------------
    FileSignalReader* createInstance (QString const& file_path);

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
    QSharedPointer<GDFBasicHeader> header_;
    mutable QMap<ChannelID, QSharedPointer<DataBlock const> > channel_map_;
};

} // namespace

#endif // GDF_FILE_SIGNAL_READER_H
