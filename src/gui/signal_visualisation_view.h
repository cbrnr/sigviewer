#ifndef SIGNAL_VISUALISATION_VIEW_H
#define SIGNAL_VISUALISATION_VIEW_H

#include <QPainter>

namespace BioSig_
{

class SignalVisualisationView
{
public:
    virtual void renderVisibleScene (QPainter* destination) const = 0;
};

}

#endif // SIGNAL_VISUALISATION_VIEW_H
