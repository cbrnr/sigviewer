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
