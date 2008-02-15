// gdf_1_header.h

#ifndef GDF_1_HEADER_H
#define GDF_1_HEADER_H

#include "gdf_event.h"

#include <QVector>

namespace BioSig_
{

class GDFSignalHeader;

// GDFHeader
class GDF1Header
{
public:
    enum EventTableType
    {
        NORMAL_EVENT_TABLE = 1,
        EXTENDED_EVENT_TABLE = 3
    };

    enum
    {
        SIZE = 256
    };

    GDF1Header();
    ~GDF1Header();

    void resetGDF1Header();

    typedef QVector<GDFSignalHeader> GDFSignalHeaderVector;

    char gdf_version_id_[8];
    char gdf_patient_id_[80];
    char gdf_recording_id_[80];
    char gdf_start_recording_[16]; // YYYYMMDDhhmmsscc
    int64 gdf_header_size_;
    uint64 gdf_equipment_provider_id_;
    uint64 gdf_labratory_id_;
    uint64 gdf_technican_id_;
    char gdf_resered_[20];
    int64 gdf_number_data_records_; // -1 if unknown
    uint32 gdf_duration_data_record_[2]; // rational - nominator, denominator
    uint32 gdf_number_signals_;
    uint8 gdf_event_table_type_; 
    uint8 gdf_event_table_sample_rate_[3];
    uint32 gdf_number_events_;
    GDFSignalHeaderVector gdf_sig_vector_;
};

} // namespace BioSig_

#endif
