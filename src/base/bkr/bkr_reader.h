#ifndef BKR_READER_H_
#define BKR_READER_H_

#include "../file_signal_reader.h"
#include "bkr_header.h"

#include <QFile>

namespace BioSig_
{

// BKR reader
class BKRReader : public FileSignalReader,
                  private BKRHeader
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
    virtual void loadEvents(SignalEventVector& event_vector);
    virtual void loadRawRecords(float64** record_data, uint32 start_record,
                                uint32 records);

private:
    // not allowed
	BKRReader(const BKRReader& src);
    const BKRReader& operator=(const BKRReader& src);

    bool loadFixedHeader(const QString& file_name);
    bool loadSignalHeaders(const QString& file_name);
//    void loadEventTableHeader();

    QFile file_;
    int8* buffer_;
};

} // namespace BioSig_

#endif /*BKR_READER_H_*/
