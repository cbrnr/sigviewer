// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#include "data_block.h"

#include "signal_processing/FFTReal.h"

#include <QDebug>

#include <algorithm>
#include <cmath>

namespace sigviewer {

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
