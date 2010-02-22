#ifndef BLOCK_VISUALISATION_ITEM_MODEL_H
#define BLOCK_VISUALISATION_ITEM_MODEL_H

#include "../base/user_types.h"
#include <QSharedPointer>

namespace BioSig_ {

class DataBlock;
class BlockVisualisationItemView;

class BlockVisualisationItemModel
{
public:
    BlockVisualisationItemModel (BlockVisualisationItemView* view,
                                 QSharedPointer<DataBlock const> data_block);

    void setPixelPerSample (float32 pixel_per_sample);

private:
    BlockVisualisationItemView* view_;
    QSharedPointer<DataBlock const> data_block_;
};

} // namespace BioSig_

#endif // BLOCK_VISUALISATION_ITEM_MODEL_H
