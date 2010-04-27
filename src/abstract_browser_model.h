#ifndef ABSTRACT_BROWSER_MODEL_H
#define ABSTRACT_BROWSER_MODEL_H

#include "base/user_types.h"
#include "gui/signal_visualisation_model.h"

#include <cassert>

namespace BioSig_
{

class AbstractBrowserModel : public SignalVisualisationModel
{

public:
    AbstractBrowserModel (float32 sample_rate,
                          std::set<EventType> const& shown_types) : SignalVisualisationModel (sample_rate, shown_types) {}

    virtual ~AbstractBrowserModel () {};

    //-------------------------------------------------------------------------
    virtual void setPixelPerXUnit (float64) = 0;
    virtual float64 getPixelPerXUnit () const {assert(0);}

    virtual void setItemsHeight (int32) = 0;

    virtual void updateLayout () = 0;
};

} // namespace BioSig_

#endif // ABSTRACT_BROWSER_MODEL_H
