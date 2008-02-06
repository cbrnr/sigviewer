// gdf_header.cpp

#include "gdf_header.h"
#include "gdf_signal_header.h"

namespace BioSig_
{

// constructor
GDFHeader::GDFHeader()
: gdf_header_size_(SIZE),
  gdf_equipment_provider_id_(0),
  gdf_labratory_id_(0),
  gdf_technican_id_(0),
  gdf_number_data_records_(0),
  gdf_number_signals_(0),
  gdf_event_table_type_(0),
  gdf_number_events_(0)
{
    memset(gdf_version_id_, ' ', sizeof(gdf_version_id_));
    memset(gdf_patient_id_, ' ', sizeof(gdf_patient_id_));
    memset(gdf_recording_id_, ' ', sizeof(gdf_recording_id_));
    memset(gdf_start_recording_, ' ', sizeof(gdf_start_recording_));
    memset(gdf_resered_, ' ', sizeof(gdf_resered_));
    gdf_duration_data_record_[0] = 0;
    gdf_duration_data_record_[1] = 1;
    gdf_event_table_sample_rate_[0] = 0;
    gdf_event_table_sample_rate_[1] = 0;
    gdf_event_table_sample_rate_[2] = 0;
}

// destructor
GDFHeader::~GDFHeader()
{
    // nothing
}

// reset
void GDFHeader::resetGDFHeader()
{
    memset(gdf_version_id_, ' ', sizeof(gdf_version_id_));
    memset(gdf_patient_id_, ' ', sizeof(gdf_patient_id_));
    memset(gdf_recording_id_, ' ', sizeof(gdf_recording_id_));
    memset(gdf_start_recording_, ' ', sizeof(gdf_start_recording_));
    gdf_header_size_ = SIZE;
    gdf_equipment_provider_id_ = 0;
    gdf_labratory_id_ = 0;
    gdf_technican_id_ = 0;
    memset(gdf_resered_, ' ', sizeof(gdf_resered_));
    gdf_number_data_records_ = 0;
    gdf_duration_data_record_[0] = 0;
    gdf_duration_data_record_[1] = 1;
    gdf_number_signals_ = 0;
    gdf_event_table_type_ = 0;
    gdf_number_events_ = 0;
    gdf_sig_vector_.clear();
    gdf_sig_vector_.squeeze();
    gdf_event_table_sample_rate_[0] = 0;
    gdf_event_table_sample_rate_[1] = 0;
    gdf_event_table_sample_rate_[2] = 0;
}

} // namespace BioSig_
