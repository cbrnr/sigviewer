#ifndef ABSTRACT_BROWSER_MODEL_H
#define ABSTRACT_BROWSER_MODEL_H

#include "base/user_types.h"
#include "gui/signal_visualisation_model.h"

namespace BioSig_
{

class AbstractBrowserModel : public SignalVisualisationModel
{

public:
    virtual ~AbstractBrowserModel () {};

    //-------------------------------------------------------------------------
    virtual void setPixelPerXUnit (float64) = 0;
    virtual float64 getPixelPerXUnit () const = 0;

    virtual void setItemsHeight (int32) = 0;

    virtual void updateLayout () = 0;
};

} // namespace BioSig_

#endif // ABSTRACT_BROWSER_MODEL_H
