#ifndef GDF_2_READER_IMPL_H_
#define GDF_2_READER_IMPL_H_

#include "gdf_reader_impl.h"
#include "gdf_2_header.h"
#include "../basic_header.h"
#include "../user_types.h"

#include <QFile>
#include <QPointer>

namespace BioSig_
{

class GDF2ReaderImpl : public GDFReaderImpl
{
public:
    GDF2ReaderImpl (QPointer<QFile> file, QPointer<BasicHeader> basic_header);
    
    virtual ~GDF2ReaderImpl ();
    
    virtual bool loadFixedHeader ();
    
    virtual bool loadSignalHeaders ();
    
    virtual bool loadEventTableHeader ();
    
    virtual void loadSignals (FileSignalReader::SignalDataBlockPtrIterator begin,
                              FileSignalReader::SignalDataBlockPtrIterator end,
                             uint32 start_record);
    
    virtual void loadEvents(FileSignalReader::SignalEventVector& event_vector);
    
    virtual bool loadRawRecords(float64** record_data, uint32 start_record,
                            uint32 records);
    
private:
    QPointer<QFile> file_;
    QPointer<BasicHeader> basic_header_;
    GDF2Header gdf_header_;
    int8 *buffer_;
    uint32 event_table_position_;
};


}

#endif /*GDF_2_READER_IMPL_H_*/
