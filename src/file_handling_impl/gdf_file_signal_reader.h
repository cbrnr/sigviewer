#ifndef GDF_FILE_SIGNAL_READER_H
#define GDF_FILE_SIGNAL_READER_H

#include "file_handling/file_signal_reader.h"

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
    virtual QSharedPointer<BasicHeader const> getBasicHeader () const {return QSharedPointer<BasicHeader const>(0);}


private:
};

} // namespace

#endif // GDF_FILE_SIGNAL_READER_H
