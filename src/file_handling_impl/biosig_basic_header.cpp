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

//-----------------------------------------------------------------------------
QMap<unsigned, QString> BiosigBasicHeader::getNamesOfUserSpecificEvents () const
{
    QMap<unsigned, QString> event_map;
    if (raw_header_->EVENT.CodeDesc)
    {
        for (unsigned index = 0; index < 255; index++)
        {
            if (raw_header_->EVENT.CodeDesc[index])
                event_map[index+1] = QString(raw_header_->EVENT.CodeDesc[index]);
        }
    }
    return event_map;
}


} // namespace BioSig_
