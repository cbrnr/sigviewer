#ifndef CNT_READER_H_
#define CNT_READER_H_
#include "../file_signal_reader.h"

#include <biosig.h>

#include <QFile>
#include <QMutex>

namespace BioSig_
{

// CNT reader
class BioSigReader : public FileSignalReader
{
public:
    BioSigReader();
    virtual ~BioSigReader();
    virtual FileSignalReader* clone();

    virtual QString open(const QString& file_name);
    virtual bool isOpen() {return biosig_header_ ? true : false;}
    virtual void close();

    virtual void loadSignals(SignalDataBlockPtrIterator begin, 
                             SignalDataBlockPtrIterator end,
                             uint32 start_record);
    
    //-------------------------------------------------------------------------
    virtual void loadEvents(SignalEventVector& event_vector);
    
    virtual QPointer<BasicHeader> getBasicHeader ();
    
    virtual HDRTYPE* getRawHeader (); 
    
    //-------------------------------------------------------------------------
    /// 
    /// @return false if an error occured
    ///         true if everything is fine
    virtual bool loadRawRecords(float64** record_data, uint32 start_record,
                                uint32 records);

private:
    // not allowed
    BioSigReader(const BioSigReader& src);
    const BioSigReader& operator=(const BioSigReader& src);

    QString loadFixedHeader(const QString& file_name);

    QPointer<BasicHeader> basic_header_;
    mutable QMutex mutex_;
    mutable QMutex biosig_access_lock_;
    HDRTYPE* biosig_header_;
};

} // namespace BioSig_

#endif /*CNT_READER_H_*/
