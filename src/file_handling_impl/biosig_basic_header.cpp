#include "biosig_basic_header.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
BiosigBasicHeader::BiosigBasicHeader (HDRTYPE* raw_header)
    : number_samples_ (raw_header->NRec * raw_header->SPR)
{
    if (raw_header->EVENT.CodeDesc)
    {
        for (unsigned index = 0; index < 255; index++)
        {
            if (raw_header->EVENT.CodeDesc[index])
                user_defined_event_map_[index+1] = QString(raw_header->EVENT.CodeDesc[index]);
        }
    }
}

//-----------------------------------------------------------------------------
uint32 BiosigBasicHeader::getNumberOfSamples () const
{
    return number_samples_;
}

//-----------------------------------------------------------------------------
QMap<unsigned, QString> BiosigBasicHeader::getNamesOfUserSpecificEvents () const
{
    return user_defined_event_map_;
}


} // namespace BioSig_
