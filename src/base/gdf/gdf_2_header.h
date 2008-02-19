#ifndef GDF_2_HEADER_H_
#define GDF_2_HEADER_H_

#include "gdf_signal_header.h"
#include "../user_types.h"

#include <QVector>

namespace BioSig_
{

struct GDF2Header
{
    enum EventTableType
    {
        NORMAL_EVENT_TABLE = 1,
        EXTENDED_EVENT_TABLE = 3
    };
    
    static uint32 const SIZE_ = 256;
    char version_id_ [8];
    char patient_identification_ [66];
    uint8 reserved_1_ [10];
    uint8 smoking_alcohol_drug_medication;
    uint8 weight_in_kg_;
    uint8 height_in_cm_;
    uint8 gender_handedness_visual_impairment_;
    char recording_identification_ [64];
    uint32 recording_location_ [4];
    uint32 startdate_and_time_of_recording_ [2];
    uint32 birthday_ [2];
    uint16 header_length_;
    uint8 reserved_2_ [6];
    uint64 equipment_provider_identification_;
    uint8 ip_address_ [6];
    uint16 head_size_in_mm_ [3];
    float32 position_reference_electrode_ [3];
    float32 position_ground_electrode_ [3];
    int64 number_of_data_records_;
    uint32 duration_data_record_rational_ [2];
    uint16 number_of_signals_;
    uint16 reserved_3_;
    
    typedef QVector<GDF2SignalHeader> GDFSignalHeaderVector;
    GDFSignalHeaderVector sig_vector_;

    uint8 mode_of_event_table_;
    uint32 number_of_events_;
    uint8 gdf_event_table_sample_rate_[3];
    
    
   
    
};
}

#endif /*GDF_2_HEADER_H_*/
