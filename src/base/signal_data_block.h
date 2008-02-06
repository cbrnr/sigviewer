// signal_data_block.h

#ifndef SIGNAL_DATA_BLOCK_H
#define SIGNAL_DATA_BLOCK_H

#include "user_types.h"

namespace BioSig_
{

// signal data block
class SignalDataBlock
{
public:
    SignalDataBlock(uint32 sub_sampl, uint32 channel_nr, uint32 nr_samples);
    ~SignalDataBlock();

    bool downsample(SignalDataBlock& dest_block, uint32 part_nr);

    const uint32 channel_number;
    const uint32 number_samples;
    const uint32 sub_sampling;

    float32* getBuffer();
    float32* getUpperBuffer();
    float32* getLowerBuffer();
    bool* getBufferValid();
    uint32 getBufferOffset() const;
    void setBufferOffset(uint32 offset);
    float32 getMinValue() const;
    float32 getMaxValue() const;
    void clearMinMax();

private:
    void calcMinMaxValue();

    uint32 buffer_offset_;   // in samples
    float32* buffer_; // mean
    float32* upper_buffer_; 
    float32* lower_buffer_; 
    bool* buffer_valid_;
    float32 min_value_;
    float32 max_value_;
    bool min_max_calculated_;
};

} // namespace BioSig_

#endif
