#ifndef GDF_READER_IMPL_H_
#define GDF_READER_IMPL_H_

#include "../basic_header.h"

#include <QPointer>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// 
/// GDFReaderImpl
///
/// abstract base class for various implementations of gdf-readers (e.g.
/// version 1.25 and version 2.00)
///

class GDFReaderImpl
{

public:
    GDFReaderImpl () {}
    
    virtual ~GDFReaderImpl() {}

    virtual bool loadFixedHeader () = 0;
    
    virtual bool loadSignalHeaders () = 0;
    
    virtual bool loadEventTableHeader () = 0;
    
//    virtual void loadSignals(SignalDataBlockPtrIterator begin, 
//                         SignalDataBlockPtrIterator end,
//                         uint32 start_record) = 0;
//    virtual void loadEvents(SignalEventVector& event_vector) = 0;
//    
//    virtual bool loadRawRecords(float64** record_data, uint32 start_record,
//                            uint32 records);

private:
    // not allowed
    GDFReaderImpl(const GDFReaderImpl& src);
    const GDFReaderImpl& operator=(const GDFReaderImpl& src);
};

}

#endif /*GDF_READER_IMPL_H_*/
