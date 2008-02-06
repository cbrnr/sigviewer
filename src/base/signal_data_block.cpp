// signal_data.cpp

#include "signal_data_block.h"
#include "math_utils.h"

#define INIT_MIN_VALUE 1E50
#define INIT_MAX_VALUE -1E50

namespace BioSig_
{

// min
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// constructor
SignalDataBlock::SignalDataBlock(uint32 sub_sampl,uint32 channel_nr,
                                 uint32 nr_samples)
: channel_number(channel_nr),
  number_samples(nr_samples),
  sub_sampling(sub_sampl),
  buffer_(new float32[nr_samples]),
  upper_buffer_(new float32[nr_samples]),
  lower_buffer_(new float32[nr_samples]),
  buffer_valid_(new bool[nr_samples]),
  min_value_(INIT_MIN_VALUE),
  max_value_(INIT_MAX_VALUE),
  min_max_calculated_(false)
{
    // nothing
}

// destructor
SignalDataBlock::~SignalDataBlock()
{
    delete[] buffer_;
    delete[] upper_buffer_;
    delete[] lower_buffer_;
    delete[] buffer_valid_;
}

// get buffer
float32* SignalDataBlock::getBuffer()
{
    return buffer_;
}

// get upper buffer
float32* SignalDataBlock::getUpperBuffer()
{
    return upper_buffer_;
}

// get lower buffer
float32* SignalDataBlock::getLowerBuffer()
{
    return lower_buffer_;
}

// buffer valid
bool* SignalDataBlock::getBufferValid()
{
    return buffer_valid_;
}

// get buffer offset
uint32 SignalDataBlock::getBufferOffset() const
{
    return buffer_offset_;
}

// set buffer offset
void SignalDataBlock::setBufferOffset(uint32 offset)
{
    buffer_offset_ = offset;
}


// downsample
bool SignalDataBlock::downsample(SignalDataBlock& dest_block, uint32 part_nr)
{
    if (channel_number != dest_block.channel_number)
    {
        return false; // wrong channel number
    }
    uint32 do_sub_sampl = dest_block.sub_sampling / sub_sampling;
    if (do_sub_sampl * sub_sampling != dest_block.sub_sampling)
    {
        return false;   // illegal subsampling
    }
    uint32 samples_per_part = number_samples / do_sub_sampl;
    if (samples_per_part * do_sub_sampl != number_samples)
    {
        return false; // illegal number_samples
    }
    uint32 dest_offset = part_nr * samples_per_part;
    if (dest_offset >= dest_block.number_samples)
    {
        return false; // part out of dest_block
    }

    if (!min_max_calculated_)
    {
        calcMinMaxValue();
    }
    if (part_nr == 0)
    {
        dest_block.buffer_offset_ = buffer_offset_ / do_sub_sampl;
        dest_block.min_max_calculated_ = true;
    }
    dest_block.min_value_ = dest_block.min_value_ > min_value_ ?
                            min_value_ : dest_block.min_value_;
    dest_block.max_value_ = dest_block.max_value_ < max_value_ ?
                            max_value_ : dest_block.max_value_;

    bool* src_valid = buffer_valid_;
    float32* src = buffer_;
    float32* src_upper = upper_buffer_;
    float32* src_lower = lower_buffer_;
    float32* dest = dest_block.buffer_ + dest_offset;
    float32* dest_upper = dest_block.upper_buffer_ + dest_offset;
    float32* dest_lower = dest_block.lower_buffer_ + dest_offset;
    bool* dest_valid = dest_block.buffer_valid_ + dest_offset;
    uint32 full_sub_sampl  = min(samples_per_part,
                                 dest_block.number_samples - dest_offset);
    if (do_sub_sampl == 2)
    {
        // faster version for downsampling 2
        while (dest < dest_block.buffer_ + dest_offset + full_sub_sampl)
        {
            if (*(src_valid++))
            {
                *dest = *(src++);
                *dest_upper = *(src_upper++);
                *dest_lower = *(src_lower++);
                if (*(src_valid++))
                {
                    *dest += *src;
                    *dest /= 2;
                    *dest_upper = max(*dest_upper, *src_upper);
                    *dest_lower = min(*dest_lower, *src_lower);
                }
                src++;
                src_upper++;
                src_lower++;
                *dest_valid = true;
            }
            else
            {
                src += 2;
                src_upper += 2;
                src_lower += 2;
                src_valid++;
                *dest_valid = false;
            }
            dest++;
            dest_upper++;
            dest_lower++;
            dest_valid++;
        }
    }
    else
    {
        // general version of downsampling
        while (dest < dest_block.buffer_ + dest_offset + full_sub_sampl)
        {
            *dest = 0.0;
            *dest_upper = -10E20;
            *dest_lower = 10E20;
            float32* end_src = src + do_sub_sampl;
            uint32 number_valid = 0;
            while(src < end_src)
            {
                if (*(src_valid++))
                {
                    number_valid++;
                    *dest += *src;
                    *dest_upper = max(*dest_upper, *src_upper);
                    *dest_lower = min(*dest_lower, *src_lower);
                }
                src++;
                src_upper++;
                src_lower++;
            }
            if (number_valid > 0)
            {
                *(dest_valid++) = true;
                *(dest++) /= number_valid;
                dest_upper++;
                dest_lower++;
            }
            else
            {
                *(dest_valid++) = false;
                dest++;
                dest_upper++;
                dest_lower++;
            }
        }
    }
    return true;
}

// get min value
float32 SignalDataBlock::getMinValue() const
{
    return min_value_;
}

// get max value
float32 SignalDataBlock::getMaxValue() const
{
    return max_value_;
}

// calc min max value
void SignalDataBlock::calcMinMaxValue()
{
    bool* src_valid = buffer_valid_;
    float32* src = buffer_;
    float32* src_end = src + number_samples;
    while (src != src_end)
    {
        if (*src_valid)
        {
            min_value_ = min_value_ > *src ? *src : min_value_;
            max_value_ = max_value_ < *src ? *src : max_value_;
        }
        src_valid++;
        src++;
    }
    min_max_calculated_ = true;
}

// clear min max
void SignalDataBlock::clearMinMax()
{
    min_value_ = INIT_MIN_VALUE;
    max_value_ = INIT_MAX_VALUE;
    min_max_calculated_ = false;
}

} // namespace BioSig_
