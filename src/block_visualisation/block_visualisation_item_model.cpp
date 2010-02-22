#include "block_visualisation_item_model.h"
#include "block_visualisation_item_view.h"
#include "../base/data_block.h"

namespace BioSig_ {

//-------------------------------------------------------------------
BlockVisualisationItemModel::BlockVisualisationItemModel (BlockVisualisationItemView* view,
                                                          QSharedPointer<DataBlock const> data_block)
    : view_ (view),
      data_block_ (data_block)
{
    view_->setDataBlock (data_block_);
}

//-------------------------------------------------------------------
void BlockVisualisationItemModel::setPixelPerSample (float32 pixel_per_sample)
{
    view_->setPixelPerSample (pixel_per_sample);
}


} // namespace BioSig_
