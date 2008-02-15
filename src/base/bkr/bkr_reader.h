#ifndef BKR_READER_H_
#define BKR_READER_H_

#include "../file_signal_reader.h"
#include "bkr_header.h"

#include <QFile>
#include <set>


namespace BioSig_
{

// BKR reader
class BKRReader : public FileSignalReader
{
public:
	BKRReader();
    virtual ~BKRReader();
    virtual FileSignalReader* clone();

    virtual bool open(const QString& file_name);
    virtual bool isOpen();
    virtual void close();

    virtual void loadSignals(SignalDataBlockPtrIterator begin, 
                             SignalDataBlockPtrIterator end,
                             uint32 start_record);
    //-------------------------------------------------------------------------
    /// there are no events in BKR-files so this method does nothing
    virtual void loadEvents(SignalEventVector& event_vector) {}
    
    virtual QPointer<BasicHeader> getBasicHeader ();
    
    //-------------------------------------------------------------------------
    /// 
    /// @return false if an error occured
    ///         true if everything is fine
    virtual bool loadRawRecords(float64** record_data, uint32 start_record,
                                uint32 records);

private:
    // not allowed
	BKRReader(const BKRReader& src);
    const BKRReader& operator=(const BKRReader& src);

    bool loadFixedHeader(const QString& file_name);

    QFile file_;
    int8* buffer_;
    QPointer<BasicHeader> basic_header_;
    BKRHeader header_;
    static std::set<uint16> const supported_versions_;
};

} // namespace BioSig_

#endif /*BKR_READER_H_*/
