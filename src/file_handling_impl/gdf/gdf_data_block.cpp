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
      current_max_ (200),
      cache_ (new GDFChannelCache (7000))
{
    loadCache (0);
}

//-------------------------------------------------------------------------------------------------
GDFDataBlock::GDFDataBlock (GDFDataBlock const& src, unsigned start_sample, unsigned length)
    : DataBlock (length, src.getSampleRatePerUnit()),
      reader_ (src.reader_),
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
    if (!cache_->hasSampleIndex (start_sample_ + index))
        loadCache (start_sample_ + index);
    current_value_ = cache_->operator [](start_sample_ + index);
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
void GDFDataBlock::loadCache (unsigned index) const
{
    size_t start = index;
    if (start > cache_->bufferSize() / 2)
        start -= cache_->bufferSize() / 2;
    else
        start = 0;
    size_t end = std::min<size_t>(start + cache_->bufferSize (), reader_->getSignalHeader_readonly(channel_).get_samples_per_record() * reader_->getMainHeader_readonly().get_num_datarecords());
    qDebug () << "GDFDataBlock::loadCache from " << start << " to " << end;
    cache_->setStartIndex (start);
    reader_->getSignal (channel_, cache_->buffer(), start, end);
}


} // namespace SigViewer_
