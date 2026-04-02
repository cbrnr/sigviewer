// © SigViewer developers
//
// License: GPL-3.0

#ifndef GDF_BASICH_EADER_H
#define GDF_BASICH_EADER_H

#include <GDF/GDFHeaderAccess.h>

#include "file_handling/basic_header.h"

namespace SigViewer_ {

//-------------------------------------------------------------------------------------------------
class GDFBasicHeader : public BasicHeader {
   public:
    //---------------------------------------------------------------------------------------------
    GDFBasicHeader(QString const& file_path, gdf::GDFHeaderAccess const& header_access);

    //---------------------------------------------------------------------------------------------
    virtual uint32 getNumberOfSamples() const;

   private:
    gdf::GDFHeaderAccess const& header_access_;
};

}  // namespace SigViewer_

#endif  // GDF_BASICH_EADER_H
