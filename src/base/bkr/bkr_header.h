// bkr_header.h

#ifndef BKR_HEADER_H
#define BKR_HEADER_H

#include "../user_types.h"

namespace BioSig_
{

// BKRHeader
struct BKRHeader
{
public:
    static uint32 const SIZE_ = 150;

    uint16 version_;
    uint16 number_channels_;
    uint16 sample_frequency_;
    uint32 number_trials_;
    uint32 samples_per_trial_;
    uint16 calibration_voltage_;
    uint16 calibration_value_;
    char electrode_code_[4];
    float32 cut_off_low_frequency_;
    float32 cut_off_high_frequency_;
    uint32 reference_periode_start_;    // in samples
    uint32 reference_periode_end_;      // in samples
    uint32 action_period_start_;        // in samples
    uint32 action_period_end_;          // in samples
    bool triggered_;
    uint32 trigger_samples_before_;
    uint32 trigger_samples_after_;
    bool horizontally_averaged_;
    uint32 horizontally_averaged_trials_;
    bool vertically_averaged_;
    uint32 vertically_averaged_trials_;
    bool compression_;
    uint32 compressed_samples_;
    bool common_average_reference_;
    bool local_average_reference_;
    bool laplace_;
    bool weighted_average_reference_;
    bool power_;
    bool average_;
    bool standrard_deviation_;
    bool bandpass_filtered_;
    bool erd_;
    bool significance_;
    bool coherence_;
    bool specturm_;
    float32 confidance_;
    bool coherence_power_specturm_;
    bool erc_;
    bool hanning_smoothed_;
    bool neuronal_network_filter_;
    uint16 number_input_units_;
    uint16 number_hidden_units_;
    uint32 number_learn_cycles_;
    float32 regression_;
    float32 learn_coefficient_;
    uint16 epoch_;
    bool erc_relative_values_;
    uint16 window_type_;
    bool kalman_filtered_;
    bool wavelet_transformed_;
    float32 lower_frequency_wavelet_;
    float32 upper_frequency_wavelet_;
    uint16 number_frequency_bands_wavelet_;
    float32 frequency_wavelet_;
    uint32 start_sample_wavelet_;
};

} // namespace BioSig_

#endif
