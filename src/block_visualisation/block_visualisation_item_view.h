#ifndef BLOCK_VISUALISATION_ITEM_VIEW_H
#define BLOCK_VISUALISATION_ITEM_VIEW_H

#include "../base/user_types.h"

#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QSharedPointer>

namespace BioSig_ {

class BlockGraphicsItem;
class DataBlock;

class BlockVisualisationItemView
{
public:
    BlockVisualisationItemView (QGraphicsScene* scene);

    void setDataBlock (QSharedPointer<DataBlock const> data_block);

    void setYPos (int y_pos);
    void setPixelPerSample (float32 pixel_per_sample);
    void setHeight (int32 height);
    int32 getHeight () const;

private:
    int32 height_;
    QGraphicsScene* scene_;
    QGraphicsItemGroup* item_group_;
    QGraphicsTextItem* label_item_;
    QGraphicsTextItem* x_unit_label_item_;
    QGraphicsTextItem* y_unit_label_item_;
    BlockGraphicsItem* data_item_;

};

} // namespace BioSig_

#endif // BLOCK_VISUALISATION_ITEM_VIEW_H
