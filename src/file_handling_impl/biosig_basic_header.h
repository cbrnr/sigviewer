#ifndef BIOSIG_BASIC_HEADER_H
#define BIOSIG_BASIC_HEADER_H

#include "../file_handling/basic_header.h"
#include "../../extern/biosig.h"

namespace BioSig_
{

class BiosigBasicHeader : public BasicHeader
{
public:
    //-------------------------------------------------------------------------
    BiosigBasicHeader (HDRTYPE* raw_header);

    //-------------------------------------------------------------------------
    virtual uint32 getNumberOfSamples () const;

    //-------------------------------------------------------------------------
    virtual QMap<unsigned, QString> getNamesOfUserSpecificEvents () const;

private:
    HDRTYPE* raw_header_;
};

} // namespace BioSig_

#endif // BIOSIG_BASIC_HEADER_H
