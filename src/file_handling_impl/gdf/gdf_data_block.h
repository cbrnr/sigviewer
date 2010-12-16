#ifndef GDF_DATA_BLOCK_H
#define GDF_DATA_BLOCK_H

#include "base/data_block.h"
#include "gdf_signal_cache.h"

#include "GDF/Reader.h"

#include <QVector>
#include <QSharedPointer>

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
class GDFDataBlock : public DataBlock
{
public:
    //---------------------------------------------------------------------------------------------
    GDFDataBlock (QSharedPointer<GDFSignalCache> cache, ChannelID channel, unsigned length, float32 sample_rate);

    //---------------------------------------------------------------------------------------------
    virtual ~GDFDataBlock ();

    //---------------------------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock> createSubBlock (uint32 start, uint32 length) const;

    //---------------------------------------------------------------------------------------------
    virtual float32 const& operator[] (uint32 index) const;

    //-------------------------------------------------------------------------
    virtual float32 getMin () const;

    //-------------------------------------------------------------------------
    virtual float32 getMax () const;

    //-------------------------------------------------------------------------
    void addDownSampledVersion (QSharedPointer<DataBlock> data, unsigned downsampling_factor);

    //-------------------------------------------------------------------------
    std::pair<QSharedPointer<DataBlock>, unsigned> getNearbyDownsampledBlock (unsigned downsampling_factor) const;

private:
    //---------------------------------------------------------------------------------------------
    GDFDataBlock (GDFDataBlock const& src, unsigned start_sample, unsigned length);

    //---------------------------------------------------------------------------------------------
    Q_DISABLE_COPY (GDFDataBlock);

    //---------------------------------------------------------------------------------------------
    ChannelID channel_;
    unsigned start_sample_;
    mutable float32 current_min_;
    mutable float32 current_max_;
    mutable float32 current_value_;

    QMap<unsigned, QSharedPointer<DataBlock> > downsampled_map_;

    mutable QSharedPointer<GDFSignalCache> cache_;
};

} // namespace SigViewer_

#endif // GDF_DATA_BLOCK_H
