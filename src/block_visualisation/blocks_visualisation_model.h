#ifndef BLOCKS_VISUALISATION_MODEL_H
#define BLOCKS_VISUALISATION_MODEL_H

#include "../base/data_block.h"

#include <QSharedPointer>

#include <list>

namespace BioSig_ {

class BlocksVisualisationView;
class BlockVisualisationItemModel;

//-----------------------------------------------------------------------------
class BlocksVisualisationModel
{
public:
    BlocksVisualisationModel (BlocksVisualisationView* view,
                              float64 pixel_per_sec,
                              float64 sample_rate);

    virtual ~BlocksVisualisationModel ();

    void visualiseBlock (QSharedPointer<DataBlock> data_block,
                         QSharedPointer<DataBlock> deviation);
    void setPixelPerSec(float64 pixel_per_sec);

    unsigned getBlockHeight () const;

    void updateLayout ();
private:
    BlocksVisualisationView* view_;
    float64 pixel_per_sec_;
    float64 sample_rate_;
    unsigned num_block_items_;
    unsigned block_height_;
    unsigned max_block_length_;

    std::list<BlockVisualisationItemModel*> items_;
};

}

#endif // BLOCKS_VISUALISATION_MODEL_H
