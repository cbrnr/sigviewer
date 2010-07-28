#include "data_block.h"

#include "signal_processing/FFTReal.h"

#include <QDebug>

#include <algorithm>
#include <cmath>

namespace BioSig_ {

//-----------------------------------------------------------------------------
DataBlock::DataBlock ()
    : length_ (0),
      start_index_ (0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
DataBlock::DataBlock (QSharedPointer<std::vector<float32> > data,
                      float32 sample_rate_per_unit)
    : data_ (data),
      length_ (data->size()),
      start_index_ (0),
      sample_rate_per_unit_ (sample_rate_per_unit)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
DataBlock::DataBlock (DataBlock const &src)
    : data_ (src.data_),
      length_ (src.length_),
      start_index_ (src.start_index_),
      label_ (src.label_),
      x_unit_label_ (src.x_unit_label_),
      y_unit_label_ (src.y_unit_label_)
{
    // nothing to do here
}


//-------------------------------------------------------------------------
QSharedPointer<DataBlock> DataBlock::createSubBlock (uint32 start,
                                                     uint32 length) const
{
    QSharedPointer<DataBlock> sub_block (new DataBlock (*this));
    sub_block->start_index_ = start;
    sub_block->length_ = length;
    return sub_block;
}

//-----------------------------------------------------------------------------
float32 const& DataBlock::operator[] (uint32 index) const
{
    return data_->at(start_index_ + index);
}

//-----------------------------------------------------------------------------
uint32 DataBlock::size () const
{
    return length_;
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
    std::vector<float32>::const_iterator start = data_->begin() + start_index_;
    std::vector<float32>::const_iterator end = data_->begin() + start_index_ + length_;

    std::vector<float32>::const_iterator min_element = std::min_element (start,
                                                                         end);
    if (min_element != end)
        return *min_element;
    else
        return 0;
}

//-----------------------------------------------------------------------------
float32 DataBlock::getMax () const
{
    std::vector<float32>::const_iterator start = data_->begin() + start_index_;
    std::vector<float32>::const_iterator end = data_->begin() + start_index_ + length_;

    std::vector<float32>::const_iterator max_element = std::max_element (start,
                                                                         end);
    if (max_element != end)
        return *max_element;
    else
        return 0;
}

////-----------------------------------------------------------------------------
//DataBlock DataBlock::getBandpassFilteredBlock (float32 lower_hz_boundary, float32 upper_hz_boundary) const
//{
//    // calculate frequency spectrum
//    /*unsigned num_samples = length_;
//    double* data_in = new double[num_samples];
//    for (unsigned x = 0; x < num_samples; x++)
//        data_in[x] = data_->at(start_index_ + x);

//    Complex* data_out = fftwpp::FFTWComplex((num_samples / 2) + 1);

//    fftwpp::rcfft1d forward (num_samples, data_in, data_out);
//    forward.fft0Normalized (data_in, data_out);

//    float32 hz_step_size = (static_cast<float32>(sample_rate_per_unit_) / 2.0) / (static_cast<float32>(num_samples) / 2.0);

//    float32 frequency = 0;
//    for (unsigned index = 1; index < (num_samples / 2); index++)
//    {
//        frequency += hz_step_size;
//        if (frequency < lower_hz_boundary || frequency > upper_hz_boundary)
//        {
//            //data_out[index] =
//            //data_out[index].imag() = 0;
//            //data_out[index].real() = 0;
//            data_out[index] = 0;
//        }
//    }

//    fftwpp::crfft1d backward (num_samples, data_out, data_in);
//    backward.fft (data_out, data_in);

//    QSharedPointer<std::vector<float32> > band_passed_data (new std::vector<float32>);
//    for (unsigned x = 0; x < num_samples; x++)
//        band_passed_data->push_back (data_in[x]);// = data_in[x] = data_[x];

//    DataBlock band_passed_block (band_passed_data, sample_rate_per_unit_);

//    delete[] data_in;
//    fftwpp::FFTWdelete(data_out);

//    return band_passed_block;*/
//    return DataBlock ();
//}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> DataBlock::createPowerSpectrum () const
{
    unsigned num_samples = length_;

    unsigned fft_samples = 1;
    while (fft_samples < num_samples)
        fft_samples *= 2;

    FFTReal::flt_t* in = new FFTReal::flt_t [fft_samples];
    FFTReal::flt_t* out = new FFTReal::flt_t [fft_samples];
    double factor = 0;
    for (unsigned x = 0; x < fft_samples; x++)
    {
        if (((x < ((fft_samples - num_samples)) / 2) ||
             (x > (fft_samples - (((fft_samples - num_samples) / 2))))))
            in[x] = 0;
        else
        {
            if (x * 2 < fft_samples)
                factor += (2.0 / num_samples);
            else
                factor -= (2.0 / num_samples);

            in[x] = data_->at(start_index_+x-((fft_samples - num_samples) / 2)) * factor;
        }
    }
    FFTReal fft_object (fft_samples);
    fft_object.do_fft(out, in);
    fft_object.rescale (out);

    QSharedPointer<std::vector<float32> > spectrum_data (new std::vector<float32>);
    for (unsigned index = 0; index < fft_samples ; index++)
    {
        spectrum_data->push_back (log10(pow(out[index], 2) + pow(out[(fft_samples/2)+index], 2)));
    }
    delete[] in;
    delete[] out;
    return QSharedPointer<DataBlock const> (new DataBlock (spectrum_data, static_cast<float32>(fft_samples) / sample_rate_per_unit_));
/*
    double* data_in = new double[num_samples * 2];
    for (unsigned x = 0; x < num_samples; x++)
        data_in[x] = data_->at(start_index_ + x);

    unsigned out_length = (num_samples / 2);

    Complex* data_out = fftwpp::FFTWComplex(out_length * 4);

    fftwpp::rcfft1d forward (num_samples, data_in, data_out);
    forward.fft0Normalized (data_in, data_out);

    QSharedPointer<std::vector<float32> > spectrum_data (new std::vector<float32>);
    for (unsigned index = 1; index < out_length + 1; index++)
    {
        spectrum_data->push_back (log10(pow(data_out[index].real(),2) + pow(data_out[index].imag(), 2)));
    }
    return QSharedPointer<DataBlock const> (new DataBlock (spectrum_data, static_cast<float32>(num_samples) / sample_rate_per_unit_));*/
}

//-----------------------------------------------------------------------------
DataBlock DataBlock::calculateMean (std::list<QSharedPointer<DataBlock const> > const &data_blocks)
{
    DataBlock mean_block;
    if (data_blocks.size() == 0)
        return mean_block;

    std::list<QSharedPointer<DataBlock const> >::const_iterator it = data_blocks.begin();
    QSharedPointer<std::vector<float32> > mean (new std::vector<float32>);
    float32 sample_rate = (*it)->sample_rate_per_unit_;
    float32 tmp_mean = 0;
    for (unsigned index = 0; index < (*(data_blocks.begin()))->size(); index++)
    {
        it = data_blocks.begin();
        tmp_mean = 0;
        while (it != data_blocks.end())
        {
            tmp_mean += (**it)[index];
            ++it;
        }
        mean->push_back(tmp_mean / data_blocks.size());
    }
    return DataBlock (mean, sample_rate);
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock> DataBlock::calculateStandardDeviation (std::list<QSharedPointer<DataBlock const> > const &data_blocks)
{
    return calculateStandardDeviationImpl (data_blocks, calculateMean(data_blocks));
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock> DataBlock::calculateStandardDeviation (std::list<QSharedPointer<DataBlock const> > const &data_blocks,
                                                 DataBlock const &means)
{
    return calculateStandardDeviationImpl (data_blocks, means);
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock> DataBlock::calculateStandardDeviationImpl (std::list<QSharedPointer<DataBlock const> >
                                                     const & data_blocks,
                                                     DataBlock const& means)
{
    QSharedPointer<std::vector<float32> > stddev (new std::vector<float32>);
    if (data_blocks.size() == 0)
        return QSharedPointer<DataBlock>(0);

    std::list<QSharedPointer<DataBlock const> >::const_iterator it = data_blocks.begin();
    float32 sample_rate = (*it)->sample_rate_per_unit_;
    float32 tmp_stddev = 0;
    for (unsigned index = 0; index < (*(data_blocks.begin()))->size(); index++)
    {
        it = data_blocks.begin();
        tmp_stddev = 0;
        float32 mean = means[index];
        while (it != data_blocks.end())
        {
            tmp_stddev += pow(((**it)[index] - mean), 2);
            ++it;
        }
        stddev->push_back(sqrt(tmp_stddev / data_blocks.size()));
    }
    QSharedPointer<DataBlock> stddev_block (new DataBlock (stddev, sample_rate));
    return stddev_block;
}

//-------------------------------------------------------------------------
//unsigned DataBlock::getLengthOfSmallestBlock (std::list<DataBlock> const &data_blocks)
//{
//    std::list<DataBlock>::const_iterator it = data_blocks.begin();
//    //for (it = data_blocks.begin(); it != data_blocks.end(); ++it)
//}



}
