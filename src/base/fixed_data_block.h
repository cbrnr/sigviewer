#ifndef FIXED_DATA_BLOCK_H
#define FIXED_DATA_BLOCK_H

#include "data_block.h"

#include <list>
#include <string>

#include <QVector>
#include <QSharedPointer>

namespace SigViewer_ {

//-------------------------------------------------------------------------
/// FixedDataBlock
///
/// basic class to store data
class FixedDataBlock : public DataBlock
{
public:
    //-------------------------------------------------------------------------
    /// @param sample_rate_per_unit as a data_block must not represent data which
    ///                             is associated to time, the sample_rate is given
    ///                             in "per unit" (e.g. "s" or "hz", etc.)
    FixedDataBlock (QSharedPointer<QVector<float32> > data, float64 sample_rate_per_unit);

    //-------------------------------------------------------------------------
    virtual ~FixedDataBlock () {}

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock> createSubBlock (size_t start, size_t length) const;

    //-------------------------------------------------------------------------
    virtual float32 const& operator[] (size_t index) const;

    //-------------------------------------------------------------------------
    virtual float32 getMin () const;

    //-------------------------------------------------------------------------
    virtual float32 getMax () const;

    //---------------------------------------------------------------------------------------------
    static QSharedPointer<DataBlock const> createPowerSpectrum (QSharedPointer<DataBlock const> data_block);

    //---------------------------------------------------------------------------------------------
    static QSharedPointer<DataBlock> calculateMean (std::list<QSharedPointer<DataBlock const> > const &data_blocks);

    //---------------------------------------------------------------------------------------------
    static QSharedPointer<DataBlock> calculateStandardDeviation (std::list<QSharedPointer<DataBlock const> > const &data_blocks);

    //---------------------------------------------------------------------------------------------
    static QSharedPointer<DataBlock> calculateStandardDeviation (std::list<QSharedPointer<DataBlock const> > const &data_blocks,
                                                 QSharedPointer<DataBlock> means);

private:
    Q_DISABLE_COPY (FixedDataBlock);

    //---------------------------------------------------------------------------------------------
    FixedDataBlock (FixedDataBlock const& base, size_t new_start, size_t new_length);

    //---------------------------------------------------------------------------------------------
    static QSharedPointer<DataBlock> calculateStandardDeviationImpl (std::list<QSharedPointer<DataBlock const> > const &data_blocks,
                                                                      QSharedPointer<DataBlock> means);

    QSharedPointer<QVector<float32> > data_;
    size_t start_index_;

    static size_t instance_count_;
};

}

#endif // FIXED_DATA_BLOCK_H
