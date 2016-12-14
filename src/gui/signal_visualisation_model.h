#ifndef SIGNAL_VISUALISATION_MODEL_H
#define SIGNAL_VISUALISATION_MODEL_H

#include "base/sigviewer_user_types.h"
#include "base/signal_event.h"
#include "file_handling/channel_manager.h"
#include "file_handling/event_manager.h"
#include "signal_visualisation_modes.h"
#include "signal_visualisation_view.h"
#include "event_view.h"
#include "signal_view_settings.h"

#include <QObject>

#include <set>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// SignalVisualisationModel
///
/// base class for all
class SignalVisualisationModel : public QObject, public EventView
{
    Q_OBJECT
    Q_PROPERTY(int sample_position_ READ getShownPosition WRITE goToSample)
public:
    //-------------------------------------------------------------------------
    /// virtual destructor
    virtual ~SignalVisualisationModel () {}

    //-------------------------------------------------------------------------
    QSharedPointer<SignalViewSettings> getSignalViewSettings ()
                    {return signal_view_settings_;}

    //-------------------------------------------------------------------------
    QSharedPointer<SignalViewSettings const> getSignalViewSettings () const
                    {return signal_view_settings_;}

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
    virtual void scaleChannel (ChannelID id, float32 lower_value, float32 upper_value) = 0;

    //-------------------------------------------------------------------------
    virtual void scaleChannel (ChannelID id) = 0;

    //-------------------------------------------------------------------------
    virtual ChannelManager const& getChannelManager () const = 0;

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
    virtual QList<EventID> getSelectedEvents () const;

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
    virtual SignalVisualisationView const* view () const = 0;

    //-------------------------------------------------------------------------
    QWidget* infoWidget ();
public slots:
    //-------------------------------------------------------------------------
    virtual void update () {}

    //-------------------------------------------------------------------------
    void setActualEventCreationType (EventType type);

signals:
    void shownEventTypesChanged (std::set<EventType> const& shown_event_types);
    void signalHeightChanged (uint32 signal_height);
    void modeChanged (SignalVisualisationMode mode);

protected:
    SignalVisualisationModel (std::set<EventType> const& shown_types, ChannelManager const& channel_manager);
    virtual void shownEventTypesChangedImpl () = 0;
    virtual void modeChangedImpl (SignalVisualisationMode) {}

private:
    QSharedPointer<SignalViewSettings> signal_view_settings_;
    SignalVisualisationMode mode_;
    EventType event_creation_type_;
    std::set<EventType> shown_event_types_;
    unsigned sample_position_;
    ScaleMode scale_mode_;
    ChannelID selected_channel_;
    QWidget* info_widget_;
};


} // namespace SigViewer_

#endif // SIGNAL_VISUALISATION_MODEL_H
