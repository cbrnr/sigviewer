// bkr_header.h

#ifndef BKR_HEADER_H
#define BKR_HEADER_H

#include "../user_types.h"

namespace BioSig_
{

// BKRHeader
class BKRHeader
{
public:
    enum
    {
        SIZE = 150
    };

//    BKRHeader();
//    ~BKRHeader();

    void resetBKRHeader();

protected:
    uint16 bkr_version_;
    uint16 bkr_number_channels_;
    uint16 bkr_sample_frequency_;
    uint32 bkr_number_trials_;
    uint32 bkr_samples_per_trial_;
    uint16 bkr_calibration_voltage_;
    uint16 bkr_calibration_value_;
    char bkr_electrode_code_[4];
    float32 bkr_cut_off_low_frequency_;
    float32 bkr_cut_off_high_frequency_;
    uint32 bkr_reference_periode_start_;    // in samples
    uint32 bkr_reference_periode_end_;      // in samples
    uint32 bkr_action_period_start_;        // in samples
    uint32 bkr_action_period_end_;          // in samples
    bool bkr_triggered_;
    uint32 bkr_trigger_samples_before_;
    uint32 bkr_trigger_samples_after_;
    bool bkr_horizontally_averaged_;
    uint32 bkr_horizontally_averaged_trials_;
    bool bkr_vertically_averaged_;
    uint32 bkr_vertically_averaged_trials_;
    bool bkr_compression_;
    uint32 bkr_compressed_samples_;
    bool bkr_common_average_reference_;
    bool bkr_local_average_reference_;
    bool bkr_laplace_;
    bool bkr_weighted_average_reference_;
    bool bkr_power_;
    bool bkr_average_;
    bool bkr_standrard_deviation_;
    bool bkr_bandpass_filtered_;
    bool bkr_erd_;
    bool bkr_significance_;
    bool bkr_coherence_;
    bool bkr_specturm_;
    float32 bkr_confidance_;
    bool bkr_coherence_power_specturm_;
    bool bkr_erc_;
    bool bkr_hanning_smoothed_;
    bool bkr_neuronal_network_filter_;
    uint16 bkr_number_input_units_;
    uint16 bkr_number_hidden_units_;
    uint32 bkr_number_learn_cycles_;
    float32 bkr_regression_;
    float32 bkr_learn_coefficient_;
    uint16 bkr_epoch_;
    bool bkr_erc_relative_values_;
    uint16 bkr_window_type_;
    bool bkr_kalman_filtered_;
    bool bkr_wavelet_transformed_;
    float32 bkr_lower_frequency_wavelet_;
    float32 bkr_upper_frequency_wavelet_;
    uint16 bkr_number_frequency_bands_wavelet_;
    float32 bkr_frequency_wavelet_;
    uint32 bkr_start_sample_wavelet_;
};

} // namespace BioSig_

#endif
