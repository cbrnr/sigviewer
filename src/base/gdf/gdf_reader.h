// gdf_reader.h

#ifndef GDF_READER_H
#define GDF_READER_H

#include "../file_signal_reader.h"
#include "gdf_header.h"

#include <QFile>

namespace BioSig_
{

// GDF reader
class GDFReader : public FileSignalReader,
                  private GDFHeader
{
public:
    GDFReader();
    virtual ~GDFReader();
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
    GDFReader(const GDFReader& src);
    const GDFReader& operator=(const GDFReader& src);

    bool loadFixedHeader(const QString& file_name);
    bool loadSignalHeaders(const QString& file_name);
    void loadEventTableHeader();

    QFile file_;
    int8* buffer_;
};

} // namespace BioSig_

#endif
