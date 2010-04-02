#ifndef CNT_READER_H_
#define CNT_READER_H_

#include "../file_handling/file_signal_reader.h"

#include <QFile>
#include <QMutex>

namespace BioSig_
{

class BioSigReader : public FileSignalReader
{
public:
    BioSigReader ();
    BioSigReader (bool prototype_instance);
    virtual ~BioSigReader();
    virtual FileSignalReader* clone();

//-----------------------------------------------------------------------------
    virtual void setFlagOverflow(const bool overflow_detection);
    virtual QString open(const QString& file_name);
    virtual QString open(const QString& file_name, const bool overflow_detection);

    virtual void enableCaching();

    virtual bool isOpen() {return biosig_header_ ? true : false;}
    virtual void close();

    virtual void loadSignals(SignalDataBlockPtrIterator begin,
                             SignalDataBlockPtrIterator end,
                             uint32 start_record);

    //-------------------------------------------------------------------------
    virtual void loadEvents(SignalEventVector& event_vector);

    virtual QPointer<BasicHeader> getBasicHeader ();

    virtual HDRTYPE* getRawHeader ();

private:
    static BioSigReader prototype_instance_;

    // not allowed
    BioSigReader(const BioSigReader& src);
    const BioSigReader& operator=(const BioSigReader& src);

    QString loadFixedHeader(const QString& file_name);

    void doClose();

    QPointer<BasicHeader> basic_header_;
    mutable QMutex mutex_;
    mutable QMutex biosig_access_lock_;
    HDRTYPE* biosig_header_;
    static double const SAMPLE_RATE_TOLERANCE_;
    double* read_data_;
    uint32 read_data_size_;
};

} // namespace BioSig_

#endif /*CNT_READER_H_*/
