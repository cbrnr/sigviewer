#ifndef GDF_1_READER_IMPL_H_
#define GDF_1_READER_IMPL_H_

#include "gdf_reader_impl.h"
#include "gdf_1_header.h"
#include "../basic_header.h"
#include "../user_types.h"

#include <QFile>
#include <QPointer>

namespace BioSig_
{

class GDF1ReaderImpl : public GDFReaderImpl
{
public:
    GDF1ReaderImpl (QPointer<QFile> file, QPointer<BasicHeader> basic_header);
    
    virtual ~GDF1ReaderImpl ();
    
    virtual bool loadFixedHeader ();
    
    virtual bool loadSignalHeaders ();
    
    virtual bool loadEventTableHeader ();
    
    virtual void loadSignals (FileSignalReader::SignalDataBlockPtrIterator begin,
                              FileSignalReader::SignalDataBlockPtrIterator end,
                             uint32 start_record);
private:
    QPointer<QFile> file_;
    QPointer<BasicHeader> basic_header_;
    GDF1Header gdf_header_;
    int8 *buffer_;
};


}

#endif /*GDF_1_READER_IMPL_H_*/
