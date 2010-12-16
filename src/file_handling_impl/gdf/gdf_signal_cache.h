#ifndef GDF_SIGNAL_CACHE_H
#define GDF_SIGNAL_CACHE_H

#include "../signal_cache.h"

#include "GDF/Reader.h"

#include <QVector>
#include <QMutex>
#include <QMap>
#include <QThread>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class GDFSignalCache : public SignalCache
{
public:
    GDFSignalCache (gdf::Reader* reader);
    virtual ~GDFSignalCache ();

    virtual float32 getSample (ChannelID channel, unsigned sample_index);

private:
    bool hasSampleIndex (unsigned cache_index, ChannelID channel, unsigned sample_index) const;
    void rebuildCache (unsigned cache_index, ChannelID channel, unsigned sample_index);
    unsigned createNewThreadCacheIndex ();


    gdf::Reader* reader_;
    static unsigned const CACHE_SIZE_PER_CHANNEL_;

    QList<QVector<double*> > channel_buffers_;
    QVector<QVector<unsigned> > start_;
    QVector<QVector<unsigned> > cyclic_start_;
    QMap<QThread*, unsigned> thread_cache_index_map_;
    QMutex rebuild_mutex_;
};

}

#endif // GDF_SIGNAL_CACHE_H
