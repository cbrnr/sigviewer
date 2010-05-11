#ifndef SIGNAL_VISUALISATION_MODEL_H
#define SIGNAL_VISUALISATION_MODEL_H

#include "../base/user_types.h"
#include "../base/signal_event.h"
#include "../file_handling/channel_manager.h"
#include "../file_handling/event_manager.h"
#include "signal_visualisation_modes.h"

#include <QObject>

#include <set>
#include <cassert>

namespace BioSig_
{

enum ZoomDimension
{
    ZOOM_VERTICAL,
    ZOOM_HORIZONTAL,
    ZOOM_BOTH
};

//-----------------------------------------------------------------------------
/// SignalVisualisationModel
///
/// base class for all
class SignalVisualisationModel : public QObject
{
    Q_OBJECT
public:

    virtual ~SignalVisualisationModel () {}

    virtual std::set<ChannelID> getShownChannels () const = 0;
    virtual void setShownChannels (std::set<ChannelID> const& shown_channels) = 0;

    void setMode (SignalVisualisationMode mode);
    SignalVisualisationMode getMode () const;

    void setPixelPerSample (float32 pixel_per_sample);
    float32 getPixelPerSample () const;
    float32 getSampleRate () const;

    //-------------------------------------------------------------------------
    virtual void scaleChannel (ChannelID id, float32 lower_value, float32 upper_value) = 0;

    //-------------------------------------------------------------------------
    virtual void autoScaleChannel (ChannelID id) = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<ChannelManager const> getChannelManager () const = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager const> getEventManager () const = 0;

    //-------------------------------------------------------------------------
    unsigned getSignalHeight () const;

    //-------------------------------------------------------------------------
    void setSignalHeight (unsigned height);

    //-------------------------------------------------------------------------
    /// @return the height of the signal viewport
    virtual unsigned getShownHeight () const = 0;

    //-------------------------------------------------------------------------
    /// @return the amount of pixels whon of the signal
    virtual unsigned getShownSignalWidth () const = 0;

    //-------------------------------------------------------------------------
    /// @return the number of sample which is shown on the left side
    virtual unsigned getShownPosition () const = 0;

    //-------------------------------------------------------------------------
    /// sets the view that the given sample is on the left side
    virtual void goToSample (unsigned sample) = 0;

    //-------------------------------------------------------------------------
    /// @return the id of the currently selected signal event
    virtual EventID getSelectedEvent () const;

    //-------------------------------------------------------------------------
    std::set<EventType> getShownEventTypes () const;

    //-------------------------------------------------------------------------
    virtual void setShownEventTypes (std::set<EventType> const& event_types);

    //-------------------------------------------------------------------------
    virtual void selectEvent (EventID) = 0;

    //-------------------------------------------------------------------------
    EventType getActualEventCreationType () const;

    //-------------------------------------------------------------------------
    void setAutoScaleMode (ScaleMode scale_mode);

    //-------------------------------------------------------------------------
    ScaleMode getAutoScaleMode () const;

    virtual void updateLayout () = 0;

public slots:
    void setActualEventCreationType (EventType type);

signals:
    void pixelPerSampleChanged (float32 pixel_per_sample, float32 sample_rate);
    void shownEventTypesChanged (std::set<EventType> const& shown_event_types);
    void signalHeightChanged (uint32 signal_height);
    void modeChanged (SignalVisualisationMode mode);

protected:
    SignalVisualisationModel (float32 sample_rate, std::set<EventType> const& shown_types);
    virtual void shownEventTypesChangedImpl () = 0;

private:
    float32 pixel_per_sample_;
    float32 sample_rate_;
    SignalVisualisationMode mode_;
    EventType event_creation_type_;
    std::set<EventType> shown_event_types_;
    unsigned signal_height_;
    ScaleMode scale_mode_;
};


} // namespace BioSig_

#endif // SIGNAL_VISUALISATION_MODEL_H
