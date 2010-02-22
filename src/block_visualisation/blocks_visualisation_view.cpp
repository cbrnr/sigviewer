#include "blocks_visualisation_view.h"
#include "block_graphics_item.h"
#include "block_visualisation_item_view.h"

#include <QScrollBar>
#include <QGridLayout>

namespace BioSig_ {

//-----------------------------------------------------------------------------
BlocksVisualisationView::BlocksVisualisationView (QWidget* parent)
    : QFrame (parent)
{
    resize(parent->contentsRect().width(), parent->contentsRect().height());
    graphics_scene_ = new QGraphicsScene (0, 0, parent->contentsRect().width(), parent->contentsRect().height(), this);
    graphics_view_ = new QGraphicsView(graphics_scene_, this);
    graphics_view_->centerOn (0,0);
    graphics_view_->setAcceptDrops (false);
    graphics_view_->setDragMode (QGraphicsView::ScrollHandDrag);
    //graphics_view_->horizontalScrollBar()->hide();
    //graphics_view_->verticalScrollBar()->hide();   


    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget (graphics_view_, 1, 1);
}

//-----------------------------------------------------------------------------
BlockVisualisationItemView* BlocksVisualisationView::createBlockVisualisationItemView ()
{
    static int count = 0;
    BlockVisualisationItemView* vis = new BlockVisualisationItemView (graphics_scene_);
    vis->setYPos (count * 130);
    count++;
    return vis;
}

//-----------------------------------------------------------------------------
void BlocksVisualisationView::addBlockGraphicsItem (BlockGraphicsItem* block_graphics_item)
{
    graphics_scene_->addItem (block_graphics_item);
    graphics_scene_->update();
    graphics_view_->update();
}

//-----------------------------------------------------------------------------
void BlocksVisualisationView::fitScene ()
{
    graphics_scene_->setSceneRect(graphics_scene_->itemsBoundingRect());
}


}
