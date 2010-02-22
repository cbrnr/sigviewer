#include "blocks_visualisation_model.h"
#include "block_visualisation_item_model.h"
#include "blocks_visualisation_view.h"


namespace BioSig_ {

//-----------------------------------------------------------------------------
BlocksVisualisationModel::BlocksVisualisationModel (BlocksVisualisationView* view,
                                                    float64 pixel_per_sec,
                                                    float64 sample_rate)
    : view_ (view),
      pixel_per_sec_ (pixel_per_sec),
      sample_rate_ (sample_rate),
      num_block_items_ (0),
      block_height_ (150), // TODO: richtig setzen!!!!
      max_block_length_ (0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
BlocksVisualisationModel::~BlocksVisualisationModel ()
{
    for (std::list<BlockVisualisationItemModel*>::iterator item = items_.begin();
         item != items_.end();
         ++item)
    {
        delete *item;
    }
}


//-----------------------------------------------------------------------------
void BlocksVisualisationModel::visualiseBlock (QSharedPointer<DataBlock> data_block,
                                               QSharedPointer<DataBlock> deviation)
{
    BlockVisualisationItemModel* item_model =
            new BlockVisualisationItemModel (view_->createBlockVisualisationItemView(),
                                             data_block);

    items_.push_back (item_model);
//    float32 pixel_per_sample = pixel_per_sec_ / sample_rate_;
//    BlockGraphicsItem* block_graphics_item = new BlockGraphicsItem (data_block,
//                                                                    pixel_per_sample,
//                                                                    block_height_,
//                                                                    deviation);
//    block_graphics_item->setPos(0, num_block_items_ * block_height_);
//    view_->addBlockGraphicsItem (block_graphics_item);
//    items_.push_back (block_graphics_item);
//    num_block_items_++;
//    if (max_block_length_ < pixel_per_sample * data_block->getData().size())
//        max_block_length_ = pixel_per_sample * data_block->getData().size();
}

//-----------------------------------------------------------------------------
void BlocksVisualisationModel::setPixelPerSec(float64 pixel_per_sec)
{
    pixel_per_sec_ = pixel_per_sec;
    float32 pixel_per_sample = pixel_per_sec_ / sample_rate_;
    for (std::list<BlockVisualisationItemModel*>::iterator it = items_.begin();
         it != items_.end();
         ++it)
    {
        (*it)->setPixelPerSample(pixel_per_sample);
    }

}

//-----------------------------------------------------------------------------
unsigned BlocksVisualisationModel::getBlockHeight () const
{
    return block_height_;
}

//-----------------------------------------------------------------------------
void BlocksVisualisationModel::updateLayout ()
{
    view_->fitScene();
}

}
