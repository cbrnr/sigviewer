// gdf_signal_header.h

#ifndef GDF_SIGNAL_HEADER_H
#define GDF_SIGNAL_HEADER_H

#include "../user_types.h"

namespace BioSig_
{

// GDFSignalHeader
struct GDFSignalHeader
{
    enum
    {
        SIZE = 256
    };

    char label[16];
    char transducer_type[80];
    char physical_dimension[8];
    float64 physical_minimum;
    float64 physical_maximum;
    int64 digital_minimum;
    int64 digital_maximum;
    char pre_filtering[80];
    uint32 samples_per_record;
    uint32 channel_type;
    char reserved[32];
};

} // namespace BioSig_

#endif
