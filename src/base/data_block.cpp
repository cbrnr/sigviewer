#include "data_block.h"

#include "signal_processing/FFTReal.h"

#include <QDebug>

#include <algorithm>
#include <cmath>

namespace SigViewer_ {

size_t DataBlock::instance_count_ = 0;

//-----------------------------------------------------------------------------
DataBlock::DataBlock (size_t length, float64 sample_rate_per_unit)
    : length_ (length),
      sample_rate_per_unit_ (sample_rate_per_unit)
{
    instance_count_++;
}

//-----------------------------------------------------------------------------
DataBlock::DataBlock (DataBlock const& src, size_t new_length)
    : length_ (new_length),
      sample_rate_per_unit_ (src.sample_rate_per_unit_),
      label_ (src.label_)
{

}


//-------------------------------------------------------------------------
DataBlock::~DataBlock ()
{
    instance_count_--;
    // qDebug () << "DataBlock::instance_count_ = " << instance_count_ << " deleting";
}

//-----------------------------------------------------------------------------
size_t DataBlock::size () const
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
float64 DataBlock::getSampleRatePerUnit () const
{
    return sample_rate_per_unit_;
}

}
