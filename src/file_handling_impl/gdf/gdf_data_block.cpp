#include "gdf_data_block.h"

#include <QDebug>

#include <cmath>

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
GDFDataBlock::GDFDataBlock (QSharedPointer<GDFSignalCache> cache, ChannelID channel, unsigned length, float32 sample_rate, int downsampling_factor)
    : DataBlock (length, sample_rate),
      channel_ (channel),
      start_sample_ (0),
      downsampling_factor_ (downsampling_factor),
      current_min_ (-200),
      current_max_ (200),
      cache_ (cache)
{

}

//-------------------------------------------------------------------------------------------------
GDFDataBlock::GDFDataBlock (GDFDataBlock const& src, unsigned start_sample, unsigned length)
    : DataBlock (length, src.getSampleRatePerUnit()),
      channel_ (src.channel_),
      start_sample_ (start_sample),
      downsampling_factor_ (src.downsampling_factor_),
      current_min_ (src.current_min_),
      current_max_ (src.current_max_),
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
    return cache_->getSample (channel_, start_sample_ + index);
//    if (!cache_->hasSampleIndex (start_sample_ + index))
//        loadCache (start_sample_ + index);
//    current_value_ = cache_->operator [](start_sample_ + index);
//    return current_value_;
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


} // namespace SigViewer_
