// gdf_event.h

#ifndef GDF_EVENT_H
#define GDF_EVENT_H

#include "../user_types.h"

namespace BioSig_
{

// GDFEvent
struct GDFEvent
{
    enum
    {
        SIZE_TYPE_1 = 6,
        SIZE_TYPE_3 = 12
    };

    uint32 position; // in samples
    uint16 type;

    // optional - only if event table type is 3
    uint16 channel; 
    uint32 duration;
};

// less operator
bool operator<(const GDFEvent& a, const GDFEvent& b);

} // namespace BioSig_

#endif
