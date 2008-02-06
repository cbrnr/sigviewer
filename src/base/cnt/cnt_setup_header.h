// cnt_setup_header.h

#include "../user_types.h"

#include <vector>

#ifndef CNT_SETUP_HEADER_H
#define CNT_SETUP_HEADER_H

namespace BioSig_
{

// CNTSetupHeader
class CNTSetupHeader
{
public:
    CNTSetupHeader();
    ~CNTSetupHeader();

    void resetCNTHeader();

private:
    char cnt_revision_[20];
    // offset 0x14
    char cnt_type_; // AVG=0, EEG=1, etc
    // offset 0x3d
    char cnt_doctor_id_[20];
  // offset 0x65
    char cnt_hospital_id_[20];
    char cnt_patient_name_[20];
    uint16 cnt_patient_age_;
    char cnt_patient_sex_;          // Male='M', Female='F'
    char cnt_patient_handedness_;   // Mixed='M',Rt='R', lft='L'
    char cnt_medication_[20];
    char cnt_classification_[20];
    // offset 0xe1
    char cnt_session_date_[10]; 
    char cnt_session_time_[12]; 
    // offset 0x161
    int32 cnt_number_events_;
    // offset 0x172
    uint16 cnt_number_channels_;
    // offset 0x178
    unsigned short cnt_d2a_rate_;
    float64 cnt_scale_;
    // offset 0x1b6
    float32 cnt_amplifier_sensitivity_;
    // offset 0x1ba
    char cnt_low_pass_;
    char cnt_high_pass_;
    char cnt_notch_;
    // offset 0x1d8
    char cnt_trigger_type_;
    // offset 0x1f1
    float32 cnt_display_minimum_;
    float32 cnt_display_maximum_;
    // offset 0x2aa
    char cnt_notch_filter_;
    // offset 0x360
    int64 cnt_number_samples_;
    char cnt_filter_flag_;
    float32 cnt_low_cut_off_;
    // offset 0x36b
    float32 cnt_high_cut_off_;
    // offset 0x371
    char cnt_filter_type_;  // Bandpass=0 Notch=1 Highpass=2 Lowpass=3
    // offset 0x376
    int64 cnt_eventtable_position_;
};

} // namespace BioSig_

#endif
