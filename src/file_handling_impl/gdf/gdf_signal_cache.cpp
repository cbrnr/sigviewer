#include "gdf_signal_cache.h"

#include <QDebug>
#include <QMutexLocker>

#include <limits>

namespace SigViewer_
{

unsigned const GDFSignalCache::CACHE_SIZE_PER_CHANNEL_ = 5000;

//-----------------------------------------------------------------------------
GDFSignalCache::GDFSignalCache (gdf::Reader* reader)
    : reader_ (reader)
{
    qDebug () << "GDFSignalCache::GDFSignalCache";
}

//-----------------------------------------------------------------------------
GDFSignalCache::~GDFSignalCache ()
{
    foreach (QVector<double*> channel_buffer, channel_buffers_)
    {
        foreach (double* double_buffer, channel_buffer)
        {
            delete[] double_buffer;
        }
    }
}

//-----------------------------------------------------------------------------
float32 GDFSignalCache::getSample (ChannelID channel, unsigned sample_index)
{
    QMutexLocker locker (&rebuild_mutex_);

    if (!thread_cache_index_map_.contains (QThread::currentThread()))
        createNewThreadCacheIndex ();
    unsigned cache_index = thread_cache_index_map_[QThread::currentThread()];

    if (!hasSampleIndex (cache_index, channel, sample_index))
        rebuildCache (cache_index, channel, sample_index);
    return channel_buffers_[cache_index][channel][((sample_index - start_[cache_index][channel]) + cyclic_start_[cache_index][channel]) % CACHE_SIZE_PER_CHANNEL_];
}

//-----------------------------------------------------------------------------
bool GDFSignalCache::hasSampleIndex (unsigned cache_index, ChannelID channel, unsigned sample_index) const
{
    return (sample_index >= start_[cache_index][channel]) && (sample_index < (start_[cache_index][channel] + CACHE_SIZE_PER_CHANNEL_));
}

//-----------------------------------------------------------------------------
void GDFSignalCache::rebuildCache (unsigned cache_index, ChannelID /*channel*/, unsigned sample_index)
{
//    QMutexLocker locker (&rebuild_mutex_);

    size_t start = sample_index;
//    if (start > CACHE_SIZE_PER_CHANNEL_ / 2)
//        start -= CACHE_SIZE_PER_CHANNEL_ / 2;
//    else
//        start = 0;

    qDebug () << "GDFSignalCache::rebuildCache " << cache_index << " for thread " << thread_cache_index_map_.key (cache_index) << "; start = " << start;

    reader_->enableCache (true);
    //reader_->initCache();

    for (int index = 0; index < channel_buffers_[cache_index].size (); index++)
    {
//        size_t old_start = start_[cache_index][index];
//        size_t old_end = std::min<size_t>(old_start + CACHE_SIZE_PER_CHANNEL_, reader_->getSignalHeader_readonly(index).get_samples_per_record() * reader_->getMainHeader_readonly().get_num_datarecords());
//        unsigned old_cyclic_start = cyclic_start_[cache_index][index];

        start_[cache_index][index] = start;
        size_t end = std::min<size_t>(start + CACHE_SIZE_PER_CHANNEL_, reader_->getSignalHeader_readonly(index).get_samples_per_record() * reader_->getMainHeader_readonly().get_num_datarecords());

//        unsigned cyclic_start = 0;
//        if (start > old_start && start < old_end)
//        {
//            cyclic_start = start - old_start;
//        }
        reader_->getSignal (index, channel_buffers_[cache_index][index], start, end);
    }

    reader_->resetCache ();
}

//-----------------------------------------------------------------------------
unsigned GDFSignalCache::createNewThreadCacheIndex ()
{
//    QMutexLocker locker (&rebuild_mutex_);

    unsigned new_cache_index = channel_buffers_.size();
    qDebug () << "GDFSignalCache::createNewThreadCacheIndex for thread " << QThread::currentThread() << "; new cache index = " << new_cache_index;


    channel_buffers_.append (QVector<double*> ());
    start_.push_back (QVector<unsigned> ());
    cyclic_start_.push_back (QVector<unsigned> ());
    for (unsigned index = 0; index < reader_->getMainHeader_readonly().get_num_signals(); index++)
    {
        channel_buffers_[new_cache_index].push_back (new double[CACHE_SIZE_PER_CHANNEL_]);
        start_[new_cache_index].push_back (std::numeric_limits<unsigned>::max());
        cyclic_start_[new_cache_index].push_back (0);
    }


    thread_cache_index_map_[QThread::currentThread()] = new_cache_index;
    return new_cache_index;
}



}
