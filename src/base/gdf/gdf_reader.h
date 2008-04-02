// gdf_reader.h

#ifndef GDF_READER_H
#define GDF_READER_H

#include "../file_signal_reader.h"
#include "../basic_header.h"
#include "gdf_reader_impl.h"


#include <QFile>
#include <QPointer>
#include <memory>


namespace BioSig_
{

// GDF reader
class GDFReader : public FileSignalReader
{
public:
    GDFReader();
    virtual ~GDFReader();
    virtual FileSignalReader* clone();

    virtual QString open(const QString& file_name);
    virtual bool isOpen();
    virtual void close();

    virtual void loadSignals(SignalDataBlockPtrIterator begin, 
                             SignalDataBlockPtrIterator end,
                             uint32 start_record);
    virtual void loadEvents(SignalEventVector& event_vector);
    
    virtual QPointer<BasicHeader> getBasicHeader ();
    
    virtual bool loadRawRecords(float64** record_data, uint32 start_record,
                                uint32 records);

private:
    // not allowed
    GDFReader(const GDFReader& src);
    const GDFReader& operator=(const GDFReader& src);

    QPointer<QFile> file_;
    QPointer<BasicHeader> basic_header_;
    int8* buffer_;
    std::auto_ptr<GDFReaderImpl> reader_impl_;
};

} // namespace BioSig_

#endif
