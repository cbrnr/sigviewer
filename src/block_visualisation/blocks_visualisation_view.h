#ifndef BLOCKS_VISUALISATION_VIEW_H
#define BLOCKS_VISUALISATION_VIEW_H

#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>

namespace BioSig_ {

class BlockGraphicsItem;
class BlockVisualisationItemView;

///----------------------------------------------------------------------------
/// BlocksVisualisationView
///
/// the "view"-class for visualising DataBlocks (e.g. means of events, etc.)
///
class BlocksVisualisationView : public QFrame
{
public:
    BlocksVisualisationView (QWidget* parent);

    BlockVisualisationItemView* createBlockVisualisationItemView ();
    void addBlockGraphicsItem (BlockGraphicsItem* block_graphics_item);

    void fitScene ();

private:
    QGraphicsScene* graphics_scene_;
    QGraphicsView* graphics_view_;

};

}

#endif // BLOCKS_VISUALISATION_VIEW_H
