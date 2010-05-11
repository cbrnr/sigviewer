#ifndef CNT_READER_H_
#define CNT_READER_H_

#include "../file_handling/file_signal_reader.h"

#include <QFile>
#include <QMutex>
#include <QMap>

namespace BioSig_
{

class BioSigReader : public FileSignalReader
{
public:
    BioSigReader ();
    BioSigReader (bool prototype_instance);

    virtual ~BioSigReader();
    virtual FileSignalReader* clone();

    virtual QString open(const QString& file_name);
    virtual QString open(const QString& file_name, const bool overflow_detection);

    virtual bool isOpen() {return is_open_;}
    virtual void close();

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
    void bufferAllChannels () const;

    //-------------------------------------------------------------------------
    void bufferAllEvents () const;

    //-------------------------------------------------------------------------
    static BioSigReader prototype_instance_;

    // not allowed
    BioSigReader(const BioSigReader& src);
    const BioSigReader& operator=(const BioSigReader& src);

    QString loadFixedHeader(const QString& file_name);

    void doClose () const;

    QSharedPointer<BasicHeader> basic_header_;
    mutable QMutex mutex_;
    mutable QMutex biosig_access_lock_;
    mutable HDRTYPE* biosig_header_;
    static double const SAMPLE_RATE_TOLERANCE_;
    mutable biosig_data_type* read_data_;
    mutable uint32 read_data_size_;
    mutable bool buffered_all_channels_;
    mutable bool buffered_all_events_;
    mutable bool is_open_;
    mutable QMap<ChannelID, QSharedPointer<DataBlock const> > channel_map_;
    mutable QList<QSharedPointer<SignalEvent const> > events_;
};

} // namespace BioSig_

#endif /*CNT_READER_H_*/
