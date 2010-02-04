#include "block_graphics_item.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <cmath>

#include <iostream>

namespace BioSig_ {

//-----------------------------------------------------------------------------
BlockGraphicsItem::BlockGraphicsItem (QSharedPointer<DataBlock> data,
                                      float32 pixel_per_sample,
                                      unsigned height,
                                      QSharedPointer<DataBlock> deviation)
    : data_ (data),
      deviation_ (deviation),
      pixel_per_sample_ (pixel_per_sample),
      y_zoom_ (1),
      height_ (height),
      y_offset_ (height / 2)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
    double max = fabs(data->getMax()) + fabs(data->getMin());

    if (data->getMin() >= 0)
        y_offset_ = height_;

    if (!deviation_.isNull())
        max += 2*fabs(deviation->getMax());
    else if (y_offset_ != height_)
        max *= 2;
    //max++;
    y_zoom_ = static_cast<float32>(height) / max;
}

//-----------------------------------------------------------------------------
QRectF BlockGraphicsItem::boundingRect () const
{
    return QRectF(0, 0, data_->getData().size() * pixel_per_sample_, height_);
}

//-----------------------------------------------------------------------------
void BlockGraphicsItem::setPixelPerSample (float32 pixel_per_sample)
{
    pixel_per_sample_ = pixel_per_sample;
    update ();
}



//-----------------------------------------------------------------------------
void BlockGraphicsItem::paint (QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    painter->drawRect(boundingRect());
    QRectF clip (option->exposedRect);
    painter->setClipping(true);
    painter->setClipRect(clip);

    painter->setPen(Qt::lightGray);
    painter->drawLine (clip.x(), y_offset_, clip.x() + clip.width(), y_offset_);

    int start_index = (clip.x() / pixel_per_sample_) - 1;
    unsigned end_index = ((clip.x() + clip.width()) / pixel_per_sample_) + 1;

    if (start_index < 0)
        start_index = 0;
    if (end_index > data_->getData().size())
        end_index = data_->getData().size();

    bool last_valid = false;
    float32 last_x = start_index * pixel_per_sample_;
    float32 last_y = 0;
    float32 last_y_dev = 0;
    float32 new_y = 0;
    float32 new_y_dev = 0;

    for (unsigned index = start_index;
         index < end_index;
         index++)
    {
        new_y = data_->getData()[index];
        if (!deviation_.isNull())
            new_y_dev = deviation_->getData()[index];
        if (last_valid)
        {
            painter->setPen(Qt::darkBlue);
            painter->drawLine(last_x, y_offset_ - (y_zoom_ * last_y), last_x + pixel_per_sample_, y_offset_ - (y_zoom_ * new_y));
            if (!deviation_.isNull())
            {
                painter->setPen(Qt::lightGray);
                painter->drawLine(last_x, y_offset_ - (y_zoom_ * (last_y + last_y_dev)), last_x + pixel_per_sample_, y_offset_ - (y_zoom_ * (new_y + new_y_dev)));
                painter->drawLine(last_x, y_offset_ - (y_zoom_ * (last_y - last_y_dev)), last_x + pixel_per_sample_, y_offset_ - (y_zoom_ * (new_y - new_y_dev)));
            }
        }
        else
        {
            last_valid = true;
        }

        last_x += pixel_per_sample_;
        last_y = new_y;
        if (!deviation_.isNull())
            last_y_dev = new_y_dev;
    }
}

}
