#include "block_visualisation_item_view.h"
#include "block_graphics_item.h"

#include "../base/data_block.h"

#include <QGraphicsScene>

namespace BioSig_ {

//-------------------------------------------------------------------
BlockVisualisationItemView::BlockVisualisationItemView (QGraphicsScene* scene)
    : scene_ (scene)
{
    item_group_ = new QGraphicsItemGroup (0, scene_);
}

//-------------------------------------------------------------------
void BlockVisualisationItemView::setDataBlock (QSharedPointer<DataBlock const> data_block)
{
    data_item_ = new BlockGraphicsItem (data_block, 1, 100);
    //scene_->addItem (data_item_);
    label_item_ = new QGraphicsTextItem (data_block->getLabel().c_str(), 0, scene_);
    x_unit_label_item_ = new QGraphicsTextItem (data_block->getXUnitLabel().c_str(), 0, scene_);
    data_item_->setPos(item_group_->pos());
    label_item_->setPos(item_group_->pos());
    item_group_->addToGroup (data_item_);
    item_group_->addToGroup (label_item_);
    item_group_->addToGroup (x_unit_label_item_);
    label_item_->moveBy (data_item_->boundingRect().width() + 1,
                         (data_item_->boundingRect().height() -
                          label_item_->boundingRect().height()) / 2);
    x_unit_label_item_->moveBy (data_item_->boundingRect().width() + 1,
                                data_item_->boundingRect().height() + 1);
}

//-------------------------------------------------------------------
void BlockVisualisationItemView::setYPos (int y_pos)
{
    item_group_->setPos (item_group_->x(), y_pos);
    // FIXXME: for Qt4.6 use item_group_->setY (y_pos);
}

//-------------------------------------------------------------------
void BlockVisualisationItemView::setPixelPerSample (float32 pixel_per_sample)
{
    data_item_->setPixelPerSample (pixel_per_sample);
    label_item_->setPos (data_item_->boundingRect().width() + 1, label_item_->y());
    x_unit_label_item_->setPos (data_item_->boundingRect().width() + 1, x_unit_label_item_->y());
    // FIXXME: for Qt4.6 use
    // label_item_->setX (data_item_->boundingRect().width() + 1);
    // x_unit_label_item_->setX (data_item_->boundingRect().width() + 1);
    item_group_->update ();
}

//-------------------------------------------------------------------
void BlockVisualisationItemView::setHeight (int32 height)
{
    height_ = height;
    data_item_->setHeight (height_ - x_unit_label_item_->boundingRect().height());
    label_item_->setPos (label_item_->x(),(data_item_->boundingRect().height() -
                                           label_item_->boundingRect().height()) / 2);
    x_unit_label_item_->setPos (x_unit_label_item_->x(), data_item_->boundingRect().height() + 1);
    // FIXXME: for Qt4.6 use
    // label_item_->setY ((data_item_->boundingRect().height() -
    //                     label_item_->boundingRect().height()) / 2);
    // x_unit_label_item_->setY (data_item_->boundingRect().height() + 1);
    item_group_->update ();
}

//-------------------------------------------------------------------
int32 BlockVisualisationItemView::getHeight () const
{
    return height_;
}



} // namespace BioSig_
