#include "data_block.h"

#include "../signal_processing/fftw++.h"

#include <algorithm>
#include <cmath>

namespace BioSig_ {

//-----------------------------------------------------------------------------
DataBlock::DataBlock()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
DataBlock::DataBlock (std::vector<float32> const &data,
                      float32 sample_rate_per_unit)
    : data_ (data),
      sample_rate_per_unit_ (sample_rate_per_unit)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
DataBlock::DataBlock (DataBlock const &src)
    : data_ (src.data_),
      label_ (src.label_),
      x_unit_label_ (src.x_unit_label_),
      y_unit_label_ (src.y_unit_label_)
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
void DataBlock::setData (std::vector<float32> const &data,
                         float32 sample_rate_per_unit)
{
    data_ = data;
    sample_rate_per_unit_ = sample_rate_per_unit;
}

//-----------------------------------------------------------------------------
std::vector<float32> const &DataBlock::getData () const
{
    return data_;
}

//-------------------------------------------------------------------------
std::string DataBlock::getLabel () const
{
    return label_;
}

//-------------------------------------------------------------------------
void DataBlock::setLabel (std::string const &label)
{
    label_ = label;
}

//-------------------------------------------------------------------------
void DataBlock::setXUnitLabel (std::string const &unit_label)
{
    x_unit_label_ = unit_label;
}

//-------------------------------------------------------------------------
std::string DataBlock::getXUnitLabel () const
{
    return x_unit_label_;
}

//-------------------------------------------------------------------------
void DataBlock::setYUnitLabel (std::string const &unit_label)
{
    y_unit_label_ = unit_label;
}

//-------------------------------------------------------------------------
std::string DataBlock::getYUnitLabel () const
{
    return y_unit_label_;
}

//-------------------------------------------------------------------------
float32 DataBlock::getSampleRatePerUnit () const
{
    return sample_rate_per_unit_;
}


//-----------------------------------------------------------------------------
float32 DataBlock::getMin () const
{
    std::vector<float32>::const_iterator min_element = std::min_element (data_.begin(), data_.end());
    if (min_element != data_.end())
        return *min_element;
    else
        return 0;
}

//-----------------------------------------------------------------------------
float32 DataBlock::getMax () const
{
    std::vector<float32>::const_iterator max_element = std::max_element (data_.begin(), data_.end());
    if (max_element != data_.end())
        return *max_element;
    else
        return 0;
}

//-----------------------------------------------------------------------------
DataBlock DataBlock::getBandpassFilteredBlock (float32 lower_hz_boundary, float32 upper_hz_boundary) const
{
    // calculate frequency spectrum
    unsigned num_samples = data_.size();
    double* data_in = new double[num_samples];
    for (unsigned x = 0; x < num_samples; x++)
        data_in[x] = data_[x];

    Complex* data_out = FFTWComplex((num_samples / 2) + 1);

    rcfft1d forward (num_samples, data_in, data_out);
    forward.fft0Normalized (data_in, data_out);

    float32 hz_step_size = (static_cast<float32>(sample_rate_per_unit_) / 2.0) / (static_cast<float32>(num_samples) / 2.0);

    float32 frequency = 0;
    for (unsigned index = 1; index < (num_samples / 2); index++)
    {
        frequency += hz_step_size;
        if (frequency < lower_hz_boundary || frequency > upper_hz_boundary)
        {
            //data_out[index] =
            //data_out[index].imag() = 0;
            //data_out[index].real() = 0;
            data_out[index] = 0;
        }
    }

    crfft1d backward (num_samples, data_out, data_in);
    backward.fft (data_out, data_in);

    std::vector<float32> band_passed_data;
    for (unsigned x = 0; x < num_samples; x++)
        band_passed_data.push_back (data_in[x]);// = data_in[x] = data_[x];

    DataBlock band_passed_block (band_passed_data, sample_rate_per_unit_);

    delete[] data_in;
    FFTWdelete(data_out);

    return band_passed_block;
}



//-----------------------------------------------------------------------------
DataBlock DataBlock::calculateMean (std::list<DataBlock> const &data_blocks)
{
    DataBlock mean_block;
    if (data_blocks.size() == 0)
        return mean_block;

    std::list<DataBlock>::const_iterator it = data_blocks.begin();
    mean_block.sample_rate_per_unit_ = it->sample_rate_per_unit_;
    float32 tmp_mean = 0;
    for (unsigned index = 0; index < (*(data_blocks.begin())).data_.size(); index++)
    {
        it = data_blocks.begin();
        tmp_mean = 0;
        while (it != data_blocks.end())
        {
            tmp_mean += it->data_[index];
            ++it;
        }
        mean_block.data_.push_back(tmp_mean / data_blocks.size());
    }
    return mean_block;
}

//-------------------------------------------------------------------------
DataBlock DataBlock::calculateStandardDeviation (std::list<DataBlock> const &data_blocks)
{
    return calculateStandardDeviationImpl (data_blocks, calculateMean(data_blocks));
}

//-------------------------------------------------------------------------
DataBlock DataBlock::calculateStandardDeviation (std::list<DataBlock> const &data_blocks,
                                                 DataBlock const &means)
{
    return calculateStandardDeviationImpl (data_blocks, means);
}

//-------------------------------------------------------------------------
DataBlock DataBlock::calculateStandardDeviationImpl (std::list<DataBlock> const &data_blocks,
                                                     DataBlock const &means)
{
    DataBlock stddev_block;
    if (data_blocks.size() == 0)
        return stddev_block;

    std::list<DataBlock>::const_iterator it = data_blocks.begin();
    stddev_block.sample_rate_per_unit_ = it->sample_rate_per_unit_;
    float32 tmp_stddev = 0;
    for (unsigned index = 0; index < (*(data_blocks.begin())).data_.size(); index++)
    {
        it = data_blocks.begin();
        tmp_stddev = 0;
        float32 mean = means.getData()[index];
        while (it != data_blocks.end())
        {
            tmp_stddev += pow((it->data_[index] - mean), 2);
            ++it;
        }
        stddev_block.data_.push_back(sqrt(tmp_stddev / data_blocks.size()));
    }
    return stddev_block;
}

//-------------------------------------------------------------------------
//unsigned DataBlock::getLengthOfSmallestBlock (std::list<DataBlock> const &data_blocks)
//{
//    std::list<DataBlock>::const_iterator it = data_blocks.begin();
//    //for (it = data_blocks.begin(); it != data_blocks.end(); ++it)
//}



}
