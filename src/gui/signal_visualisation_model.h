#ifndef SIGNAL_VISUALISATION_MODEL_H
#define SIGNAL_VISUALISATION_MODEL_H

#include "../base/sigviewer_user_types.h"
#include "../base/signal_event.h"
#include "../file_handling/channel_manager.h"
#include "../file_handling/event_manager.h"
#include "signal_visualisation_modes.h"
#include "signal_visualisation_view.h"
#include "event_view.h"

#include <QObject>

#include <set>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// SignalVisualisationModel
///
/// base class for all
class SignalVisualisationModel : public QObject, public EventView
{
    Q_OBJECT
    Q_PROPERTY(float pixel_per_sample_ READ getPixelPerSample WRITE setPixelPerSample)
    Q_PROPERTY(float signal_height_ READ getSignalHeight WRITE setSignalHeight)
    Q_PROPERTY(int sample_position_ READ getShownPosition WRITE goToSample)
public:
    //-------------------------------------------------------------------------
    /// virtual destructor
    virtual ~SignalVisualisationModel () {}

    //-------------------------------------------------------------------------
    virtual std::set<ChannelID> getShownChannels () const = 0;

    //-------------------------------------------------------------------------
    virtual void setShownChannels (std::set<ChannelID> const& shown_channels) = 0;

    //-------------------------------------------------------------------------
    void setMode (SignalVisualisationMode mode);

    //-------------------------------------------------------------------------
    SignalVisualisationMode getMode () const;

    //-------------------------------------------------------------------------
    /// the visualisation view takes ownership of the info_widget
    void setInfoWidget (QWidget* info_widget);

    //-------------------------------------------------------------------------
    void setPixelPerSample (float pixel_per_sample);

    //-------------------------------------------------------------------------
    float getPixelPerSample () const;

    //-------------------------------------------------------------------------
    virtual void scaleChannel (ChannelID id, float32 lower_value, float32 upper_value) = 0;

    //-------------------------------------------------------------------------
    virtual void scaleChannel (ChannelID id) = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<ChannelManager const> getChannelManager () const = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager const> getEventManager () const = 0;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager> getEventManager () = 0;

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
    virtual ChannelID getSelectedChannel () const;

    //-------------------------------------------------------------------------
    virtual void selectChannel (ChannelID channel);

    //-------------------------------------------------------------------------
    /// @return the id of the currently selected signal event
    virtual EventID getSelectedEvent () const;

    //-------------------------------------------------------------------------
    virtual std::set<EventType> getShownEventTypes () const;

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

    //-------------------------------------------------------------------------
    virtual void update () {}

    //-------------------------------------------------------------------------
    virtual SignalVisualisationView const* view () const = 0;

    //-------------------------------------------------------------------------
    QWidget* infoWidget ();
public slots:
    void setActualEventCreationType (EventType type);

signals:
    void pixelPerSampleChanged (float32 pixel_per_sample, float32 sample_rate);
    void shownEventTypesChanged (std::set<EventType> const& shown_event_types);
    void signalHeightChanged (uint32 signal_height);
    void modeChanged (SignalVisualisationMode mode);

protected:
    SignalVisualisationModel (std::set<EventType> const& shown_types);
    virtual void shownEventTypesChangedImpl () = 0;
    virtual void modeChangedImpl (SignalVisualisationMode) {}

private:
    float pixel_per_sample_;
    SignalVisualisationMode mode_;
    EventType event_creation_type_;
    std::set<EventType> shown_event_types_;
    unsigned signal_height_;
    unsigned sample_position_;
    ScaleMode scale_mode_;
    ChannelID selected_channel_;
    QWidget* info_widget_;
};


} // namespace BioSig_

#endif // SIGNAL_VISUALISATION_MODEL_H
