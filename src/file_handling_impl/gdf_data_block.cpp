#include "gdf_data_block.h"

#include <QDebug>

#include <cmath>

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
GDFDataBlock::GDFDataBlock (gdf::Reader* reader, ChannelID channel, int downsampling_factor)
    : DataBlock (reader->getSignalHeader_readonly (channel).get_samples_per_record() *
                 reader->getMainHeader_readonly().get_num_datarecords() /
                 downsampling_factor,
                 reader->getSignalHeader_readonly (channel).get_samplerate()),
      reader_ (reader),
      channel_ (channel),
      start_sample_ (0),
      downsampling_factor_ (downsampling_factor),
      current_min_ (-200),
      current_max_ (200)
{

}

//-------------------------------------------------------------------------------------------------
GDFDataBlock::GDFDataBlock (GDFDataBlock const& src, unsigned start_sample, unsigned length)
    : DataBlock (length, src.getSampleRatePerUnit()),
      reader_ (src.reader_),
      channel_ (src.channel_),
      start_sample_ (start_sample),
      downsampling_factor_ (src.downsampling_factor_),
      current_min_ (src.current_min_),
      current_max_ (src.current_max_)
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
    current_value_ = reader_->getSample (channel_, (start_sample_ + index) * downsampling_factor_);
    current_min_ = std::min (current_value_, current_min_);
    current_max_ = std::max (current_value_, current_max_);
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


} // namespace SigViewer_
