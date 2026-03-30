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


#include "gdf_data_block.h"

#include <QDebug>

#include <cmath>
#include <limits>

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
GDFDataBlock::GDFDataBlock (QSharedPointer<GDFSignalCache> cache, ChannelID channel, unsigned length, float32 sample_rate)
    : DataBlock (length, sample_rate),
      channel_ (channel),
      start_sample_ (0),
      current_min_ (-200),
      current_max_ (200),
      cache_ (cache)
{

}

//-------------------------------------------------------------------------------------------------
GDFDataBlock::GDFDataBlock (GDFDataBlock const& src, unsigned start_sample, unsigned length)
    : DataBlock (src, length),
      channel_ (src.channel_),
      start_sample_ (start_sample),
      current_min_ (src.current_min_),
      current_max_ (src.current_max_),
      downsampled_map_ (src.downsampled_map_),
      cache_ (src.cache_)
{

}


//-------------------------------------------------------------------------------------------------
GDFDataBlock::~GDFDataBlock ()
{

}

//-------------------------------------------------------------------------------------------------
QSharedPointer<DataBlock> GDFDataBlock::createSubBlock (uint32 start, uint32 length) const
{
    return QSharedPointer<DataBlock> (new GDFDataBlock (*this, start_sample_ + start, length));
}

//-------------------------------------------------------------------------------------------------
float32 const& GDFDataBlock::operator[] (uint32 index) const
{
    current_value_ = cache_->getSample (channel_, start_sample_ + index);
    return current_value_;
}

//-------------------------------------------------------------------------
float32 GDFDataBlock::getMin () const
{
    return current_min_;
}

//-------------------------------------------------------------------------
float32 GDFDataBlock::getMax () const
{
    return current_max_;
}

//-------------------------------------------------------------------------
void GDFDataBlock::addDownSampledVersion (QSharedPointer<DataBlock> data, unsigned downsampling_factor)
{
    downsampled_map_[downsampling_factor] = data;
}

//-------------------------------------------------------------------------
std::pair<QSharedPointer<DataBlock>, unsigned> GDFDataBlock::getNearbyDownsampledBlock (unsigned downsampling_factor) const
{
    unsigned nearest_factor = 1;
    bool search = true;
    for (nearest_factor = downsampling_factor + 1; search && (nearest_factor > 1); --nearest_factor)
        if (downsampled_map_.contains (nearest_factor - 1))
            search = false;

    if ((nearest_factor <= downsampling_factor) && (nearest_factor > 1))
    {
        return std::pair<QSharedPointer<DataBlock>, unsigned> (downsampled_map_[nearest_factor]->createSubBlock(start_sample_ / nearest_factor, size() / nearest_factor), nearest_factor);
    }
    else
        return std::pair<QSharedPointer<DataBlock>, unsigned> (createSubBlock (0, size ()), 1);
}



} // namespace SigViewer_
