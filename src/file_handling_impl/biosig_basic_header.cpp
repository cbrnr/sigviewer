#include "biosig_basic_header.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
BiosigBasicHeader::BiosigBasicHeader (HDRTYPE* raw_header)
    : raw_header_ (raw_header)
{

}

//-----------------------------------------------------------------------------
uint32 BiosigBasicHeader::getNumberOfSamples () const
{
    return raw_header_->NRec * raw_header_->SPR;
}

} // namespace BioSig_
