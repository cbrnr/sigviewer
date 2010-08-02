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
    QSharedPointer<FileSignalReader> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID channel_id,
                                                           unsigned start_sample,
                                                           unsigned length) const;

    //-------------------------------------------------------------------------
    virtual QList<QSharedPointer<SignalEvent const> > getEvents () const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader> getBasicHeader ();

private:
    //-------------------------------------------------------------------------
    QString open (QString const& file_name);

    //-------------------------------------------------------------------------
    void bufferAllChannels () const;

    //-------------------------------------------------------------------------
    void bufferAllEvents () const;

    // not allowed
    BioSigReader(const BioSigReader& src);
    const BioSigReader& operator=(const BioSigReader& src);

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
