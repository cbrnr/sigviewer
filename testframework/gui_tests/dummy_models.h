#ifndef DUMMY_MODELS_H
#define DUMMY_MODELS_H

#include "gui/main_window_model.h"
#include "gui/signal_visualisation_model.h"

class DummySignalVisualisationModel : public BioSig_::SignalVisualisationModel
{
public:
    virtual std::set<ChannelID> getShownChannels () const = 0;
    virtual void setShownChannels (std::set<ChannelID> const& shown_channels) = 0;
    virtual void scaleChannel (ChannelID id, float32 lower_value, float32 upper_value) = 0;
    virtual void scaleChannel (ChannelID id) = 0;
    virtual QSharedPointer<ChannelManager const> getChannelManager () const = 0;
    virtual QSharedPointer<EventManager const> getEventManager () const = 0;
    virtual QSharedPointer<EventManager> getEventManager () = 0;
    virtual unsigned getShownHeight () const = 0;
    virtual unsigned getShownSignalWidth () const = 0;
    virtual unsigned getShownPosition () const = 0;
    virtual void goToSample (unsigned sample) = 0;
    virtual void selectEvent (EventID) = 0;
    virtual SignalVisualisationView const* view () const = 0;
protected:
    virtual void shownEventTypesChangedImpl () = 0;
};

#endif // DUMMY_MODELS_H
