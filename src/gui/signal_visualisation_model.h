#ifndef SIGNAL_VISUALISATION_MODEL_H
#define SIGNAL_VISUALISATION_MODEL_H

#include "../base/user_types.h"

#include <set>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// SignalVisualisationModel
///
/// base class for all
class SignalVisualisationModel
{
public:
    virtual ~SignalVisualisationModel () {}

    virtual std::set<ChannelID> getShownChannels () const = 0;
    virtual bool setShownChannels (std::set<ChannelID> const& shown_channels) = 0;

    virtual void updateLayout () = 0;
};


} // namespace BioSig_

#endif // SIGNAL_VISUALISATION_MODEL_H
