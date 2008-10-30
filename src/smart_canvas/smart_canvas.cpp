// smart_canvas.cpp

#include "smart_canvas.h"

namespace BioSig_
{

// constructor
SmartCanvas::SmartCanvas(QObject* parent, bool clear_background)
: Q3Canvas(parent), //QGraphicsScene::QGraphicsScene(parent)
  clear_background_(clear_background)
{
    // nothing
}

// resize
void SmartCanvas::resize(int32 width, int32 height)
{
    int32 chunk_size = 16;
    int32 chunk_matrix_size = (width / chunk_size) * (height / chunk_size);
    while (chunk_matrix_size > MAX_CHUNK_MATRIX_SIZE)
    {
        chunk_matrix_size /= 4;
        chunk_size *= 2;
    }

    // prevent huge memory allocations
    if (chunk_size > chunkSize())
    {
        retune(chunk_size); //TODO: port retune
        Q3Canvas::resize(width, height); //QGraphicsScene::setSceneRect(0, 0, width, height)
    }
    else
    {
        Q3Canvas::resize(width, height); //QGraphicsScene::setSceneRect(0, 0, width, height)
        retune(chunk_size); //TODO: port retune
    }
}

// get last draw background clip
const QRect& SmartCanvas::getLastDrawBackgroundClip()
{
    return last_draw_background_clip_;
}

// draw Background
void SmartCanvas::drawBackground(QPainter& painter, const QRect& clip)
{
    if (clear_background_)
    {
        Q3Canvas::drawBackground(painter, clip); //QGraphicsScene::drawBackground(painter, clip);
    }
    last_draw_background_clip_ = clip;
}

} // namespace BioSig_
