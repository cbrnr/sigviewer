#ifndef GDF_SIGNAL_CACHE_H
#define GDF_SIGNAL_CACHE_H

#include "../signal_cache.h"

#include "GDF/Reader.h"

#include <QVector>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class GDFSignalCache : public SignalCache
{
public:
    GDFSignalCache (gdf::Reader* reader);
    virtual ~GDFSignalCache ();

    virtual float32 getSample (ChannelID channel, unsigned sample_index) const;

private:
    bool hasSampleIndex (ChannelID channel, unsigned sample_index) const;
    void rebuildCache (ChannelID channel, unsigned sample_index) const;


    mutable gdf::Reader* reader_;
    mutable QVector<double*> channel_buffers_;
    static unsigned const CACHE_SIZE_PER_CHANNEL_;

    mutable QVector<unsigned> start_;
};

}

#endif // GDF_SIGNAL_CACHE_H
