// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "fixed_data_block.h"

#include "signal_processing/FFTReal.h"

#include <algorithm>
#include <cmath>

namespace sigviewer
{

//-------------------------------------------------------------------------------------------------
FixedDataBlock::FixedDataBlock (QSharedPointer<QVector<float32> > data,
                                float64 sample_rate_per_unit)
    : DataBlock (data->size(), sample_rate_per_unit),
      data_ (data),
      start_index_ (0)
{
    // nothing to do here
}

//---------------------------------------------------------------------------------------------
FixedDataBlock::FixedDataBlock (FixedDataBlock const& base, size_t new_start, size_t new_length)
    : DataBlock (base, new_length),
      data_ (base.data_),
      start_index_ (new_start)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock> FixedDataBlock::createSubBlock (size_t start,
                                                          size_t length) const
{
    return QSharedPointer<DataBlock> (new FixedDataBlock (*this, start, length));
}

//-------------------------------------------------------------------------------------------------
float32 const& FixedDataBlock::operator[] (size_t index) const
{
    return data_->at(start_index_ + index);
}

//-------------------------------------------------------------------------------------------------
//! Get the minimal value in a data block, excluding any NANs
float32 FixedDataBlock::getMin () const
{
    float32 min = 0;
    for (auto const elem : *data_)
    {
        if (!std::isnan(elem))
        {
            min = elem;
            break;
        }
    }

    for (auto const elem : *data_)
    {
        if (!std::isnan(elem))
        {
            if (elem < min)
                min = elem;
        }
    }
    return min;
}

//-------------------------------------------------------------------------------------------------
//! Get the maximal value in a data block, excluding any NANs
float32 FixedDataBlock::getMax () const
{
    float32 max = 0;
    for (auto const elem : *data_)
    {
        if (!std::isnan(elem))
        {
            max = elem;
            break;
        }
    }

    for (auto const elem : *data_)
    {
        if (!std::isnan(elem))
        {
            if (elem > max)
                max = elem;
        }
    }
    return max;
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock const> FixedDataBlock::createPowerSpectrum (QSharedPointer<DataBlock const> data_block)
{
    size_t num_samples = data_block->size();

    unsigned fft_samples = 1;
    while (fft_samples < num_samples)
        fft_samples *= 2;

    FFTReal::flt_t* in = new FFTReal::flt_t [fft_samples];
    FFTReal::flt_t* out = new FFTReal::flt_t [fft_samples];
    double factor = 0;
    for (size_t x = 0; x < fft_samples; x++)
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

            in[x] = (*data_block)[x-((fft_samples - num_samples) / 2)] * factor;
        }
    }
    FFTReal fft_object (fft_samples);
    fft_object.do_fft(out, in);
    fft_object.rescale (out);

    QSharedPointer<QVector<float32> > spectrum_data (new QVector<float32>);
    for (size_t index = 0; index < (fft_samples / 2) ; index++)
    {
        spectrum_data->push_back (log10(pow(out[index], 2) + pow(out[(fft_samples/2)+index], 2)));
    }
    delete[] in;
    delete[] out;
    return QSharedPointer<DataBlock const> (new FixedDataBlock (spectrum_data, static_cast<float64>(fft_samples) / data_block->getSampleRatePerUnit()));
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
    return QSharedPointer<DataBlock const> (new DataBlock (spectrum_data, static_cast<float64>(num_samples) / sample_rate_per_unit_));*/
}

//-----------------------------------------------------------------------------
QSharedPointer<DataBlock> FixedDataBlock::calculateMean (std::list<QSharedPointer<DataBlock const> > const &data_blocks)
{
    if (data_blocks.size() == 0)
        return QSharedPointer<DataBlock> (0);

    std::list<QSharedPointer<DataBlock const> >::const_iterator it = data_blocks.begin();
    QSharedPointer<QVector<float32> > mean (new QVector<float32>);
    float64 sample_rate = (*it)->getSampleRatePerUnit ();
    float64 tmp_mean = 0;
    for (size_t index = 0; index < (*(data_blocks.begin()))->size(); index++)
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
    return QSharedPointer<DataBlock> (new FixedDataBlock (mean, sample_rate));
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock> FixedDataBlock::calculateStandardDeviation (std::list<QSharedPointer<DataBlock const> > const &data_blocks)
{
    return calculateStandardDeviationImpl (data_blocks, calculateMean(data_blocks));
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock> FixedDataBlock::calculateStandardDeviation (std::list<QSharedPointer<DataBlock const> > const &data_blocks,
                                                  QSharedPointer<DataBlock> means)
{
    return calculateStandardDeviationImpl (data_blocks, means);
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock> FixedDataBlock::calculateStandardDeviationImpl (std::list<QSharedPointer<DataBlock const> >
                                                     const & data_blocks,
                                                     QSharedPointer<DataBlock> means)
{
    QSharedPointer<QVector<float32> > stddev (new QVector<float32>);
    if (data_blocks.size() == 0)
        return QSharedPointer<DataBlock>(0);

    std::list<QSharedPointer<DataBlock const> >::const_iterator it = data_blocks.begin();
    float64 sample_rate = (*it)->getSampleRatePerUnit ();
    float64 tmp_stddev = 0;
    for (size_t index = 0; index < (*(data_blocks.begin()))->size(); index++)
    {
        it = data_blocks.begin();
        tmp_stddev = 0;
        float64 mean = (*means)[index];
        while (it != data_blocks.end())
        {
            tmp_stddev += pow(((**it)[index] - mean), 2);
            ++it;
        }
        stddev->push_back(sqrt(tmp_stddev / (data_blocks.size())));
    }
    QSharedPointer<DataBlock> stddev_block (new FixedDataBlock (stddev, sample_rate));
    return stddev_block;
}

}
