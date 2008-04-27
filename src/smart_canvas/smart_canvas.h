// smart_canvas.h

#ifndef SMART_CANVAS_H
#define SMART_CANVAS_H

#include "../base/user_types.h"

#include <q3canvas.h>

namespace BioSig_
{

// smart canvas
class SmartCanvas : public Q3Canvas
{
public:
    enum
    {
        MAX_CHUNK_MATRIX_SIZE = 25000 //1000000
    };

    enum
    {
        SMART_CANVAS_ITEM_RTTI_OFFSET = 0x500
    };

    SmartCanvas(QObject* parent = 0, bool clear_background = true);

    void resize(int32 width, int32 height);
    const QRect& getLastDrawBackgroundClip();

protected:
    void drawBackground(QPainter& painter, const QRect& clip);

private:
    QRect last_draw_background_clip_;
    bool clear_background_;
};

} // namespace BioSig_

#endif
