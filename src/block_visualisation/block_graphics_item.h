#ifndef BLOCK_GRAPHICS_ITEM_H
#define BLOCK_GRAPHICS_ITEM_H

#include "../base/data_block.h"

#include <QGraphicsItem>
#include <QSharedPointer>

namespace BioSig_ {

class BlockGraphicsItem : public QGraphicsItem
{
public:
    BlockGraphicsItem (QSharedPointer<DataBlock> data,
                       float32 pixel_per_sample,
                       unsigned height,
                       QSharedPointer<DataBlock> deviation = QSharedPointer<DataBlock>(0));

    virtual QRectF boundingRect () const;

    void setPixelPerSample (float32 pixel_per_sample);


protected:
    virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

private:
    QSharedPointer<DataBlock> data_;
    QSharedPointer<DataBlock> deviation_;
    float32 pixel_per_sample_;
    float32 y_zoom_;
    unsigned height_;
    float32 y_offset_;
};

}

#endif // BLOCK_GRAPHICS_ITEM_H
