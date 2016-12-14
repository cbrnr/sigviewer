#ifndef GDF_BASICH_EADER_H
#define GDF_BASICH_EADER_H

#include "file_handling/basic_header.h"

#include <GDF/GDFHeaderAccess.h>

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
class GDFBasicHeader : public BasicHeader
{
public:
    //---------------------------------------------------------------------------------------------
    GDFBasicHeader (QString const& file_path, gdf::GDFHeaderAccess const& header_access);

    //---------------------------------------------------------------------------------------------
    virtual uint32 getNumberOfSamples () const;

private:
    gdf::GDFHeaderAccess const& header_access_;
};

}

#endif // GDF_BASICH_EADER_H
