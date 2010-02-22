#ifndef BLOCKS_VISUALISATION_VIEW_H
#define BLOCKS_VISUALISATION_VIEW_H

#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <vector>

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

    void fitScene ();

private:
    QGraphicsScene* graphics_scene_;
    QGraphicsView* graphics_view_;
    std::vector<BlockVisualisationItemView*> block_views_;
    unsigned spacing_;
};

}

#endif // BLOCKS_VISUALISATION_VIEW_H
