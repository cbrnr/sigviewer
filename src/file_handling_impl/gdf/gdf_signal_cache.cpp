#include "gdf_signal_cache.h"

#include <QDebug>

#include <limits>

namespace SigViewer_
{

unsigned const GDFSignalCache::CACHE_SIZE_PER_CHANNEL_ = 5000;

//-----------------------------------------------------------------------------
GDFSignalCache::GDFSignalCache (gdf::Reader* reader)
    : reader_ (reader)
{
    qDebug () << "GDFSignalCache::GDFSignalCache";

    for (unsigned index = 0; index < reader_->getMainHeader_readonly().get_num_signals(); index++)
    {
        channel_buffers_.push_back (new double[CACHE_SIZE_PER_CHANNEL_]);
        start_.push_back (std::numeric_limits<unsigned>::max());
    }
}

//-----------------------------------------------------------------------------
GDFSignalCache::~GDFSignalCache ()
{
    foreach (double* channel_buffer, channel_buffers_)
    {
        delete[] channel_buffer;
    }
}

//-----------------------------------------------------------------------------
float32 GDFSignalCache::getSample (ChannelID channel, unsigned sample_index) const
{
    if (!hasSampleIndex (channel, sample_index))
        rebuildCache (channel, sample_index);
    return channel_buffers_[channel][sample_index - start_[channel]];
}

//-----------------------------------------------------------------------------
bool GDFSignalCache::hasSampleIndex (ChannelID channel, unsigned sample_index) const
{
    return (sample_index >= start_[channel]) && (sample_index < (start_[channel] + CACHE_SIZE_PER_CHANNEL_));
}

//-----------------------------------------------------------------------------
void GDFSignalCache::rebuildCache (ChannelID channel, unsigned sample_index) const
{
    static unsigned count = 0;
    size_t start = sample_index;
    if (start > CACHE_SIZE_PER_CHANNEL_ / 2)
        start -= CACHE_SIZE_PER_CHANNEL_ / 2;
    else
        start = 0;

    qDebug () << "GDFSignalCache::rebuildCache " << count++ << "; " << start;

    reader_->enableCache (true);
    //reader_->initCache();

    for (unsigned index = 0; index < channel_buffers_.size (); index++)
    {
        start_[index] = start;
        size_t end = std::min<size_t>(start + CACHE_SIZE_PER_CHANNEL_, reader_->getSignalHeader_readonly(index).get_samples_per_record() * reader_->getMainHeader_readonly().get_num_datarecords());
        reader_->getSignal (index, channel_buffers_[index], start, end);
        // qDebug () << "rebuild cache for channel " << index << " new start = " << start_[index];
    }

    reader_->resetCache ();
}



}
