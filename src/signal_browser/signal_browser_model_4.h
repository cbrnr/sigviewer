// signal_browser_model.h
#ifndef SIGNAL_BROWSER_MODEL_H_q4
#define SIGNAL_BROWSER_MODEL_H_q4

#include "../abstract_browser_model.h"
#include "../file_context.h"
#include "../tab_context.h"
#include "../file_handling/channel_manager.h"
#include "../gui/signal_visualisation_modes.h"
#include "event_graphics_item.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QPointF>

#include <map>
#include <vector>
#include <set>

namespace BioSig_
{

class SignalBrowserView;
class SignalGraphicsItem;

// signal browser model
class SignalBrowserModel : public AbstractBrowserModel
{
    Q_OBJECT

public:

    typedef QList<uint16> IntList;

    SignalBrowserModel(QSharedPointer<FileContext> file_context,
                       QSharedPointer<TabContext> tab_context);
    virtual ~SignalBrowserModel();

    virtual void setPixelPerXUnit (float64 pixel_per_sec);
    virtual float64 getPixelPerXUnit () const;

    virtual void setItemsHeight (int32 height);


    virtual void updateLayout ();

    virtual void zoom (ZoomDimension dimension, float factor);

    //-------------------------------------------------------------------------
    /// see base class
    virtual unsigned getShownSignalWidth () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual unsigned getShownPosition () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual void goToSample (unsigned sample);

    //-------------------------------------------------------------------------
    /// see base class
    virtual EventID getSelectedEvent () const;

    void setSignalBrowserView(SignalBrowserView* signal_browser_view);
    void loadSettings();
    void saveSettings();

    //settings
    std::map<std::string, bool> getHideableWidgetsVisibilities () const;
    void setHideableWidgetsVisibilities (std::map<std::string, bool> const &widgets_visiblities);
    void setXGridVisible(bool visible);
    bool getGridVisible () const;
    void setYGridVisible(bool visible);
    void setAutoZoomBehaviour (ScaleMode auto_zoom_type);
    ScaleMode getAutoZoomBehaviour () const;

    // channels
    virtual bool setShownChannels (std::set<ChannelID> const& shown_channels);
    bool isChannelShown(uint32 channel_nr) const;
    virtual std::set<ChannelID> getShownChannels () const;
    std::map<uint32, QString> getShownChannelsWithLabels () const;
    uint32 getNumberShownChannels() const;
    int32 getYPosOfChannel (uint32 channel_nr) const;

    QPointF getViewingPosition ();
    void setViewingPosition (QPointF topleft);

    // actions
    void zoomInAll();
    void zoomOutAll();

    void autoScaleAll();

    void goTo(float32 sec, int32 channel_index);
    void goToAndSelectNextEvent (bool forward);

    // get parameters
    int32 getSignalHeight();
    int32 getSignalSpacing();
    int32 getVisibleWidth();
    /*    int32 getPreferedXGirdPixelIntervall();
*/
    int32 getPreferedYGirdPixelIntervall();
    float64 getXGridPixelIntervall();

    // events
    void getShownEventTypes(IntList& event_type);
    std::set<uint16> getShownEventTypes () const;
    std::set<uint16> getDisplayedEventTypes () const;
    void setShownEventTypes(const IntList& event_type);

    EventGraphicsItem* getSelectedEventItem();
    QSharedPointer<SignalEvent const> getSelectedSignalEvent();
    void setSelectedEventToAllChannels();
    void changeSelectedEventChannel();
    void copySelectedEventToChannels();
    void changeSelectedEventType (uint16 new_type);

    EventType getActualEventCreationType () const;

    void updateEventItems ();

public slots:
    //-------------------------------------------------------------------------
    /// adds the given event
    virtual void addEventItem (QSharedPointer<SignalEvent const> event);

    //-------------------------------------------------------------------------
    /// removes the given event
    virtual void removeEventItem (EventID id);


    void setEventChanged (EventID id);
    void setActualEventCreationType (uint16 new_type);
    virtual void selectEvent (EventID id);
    void unselectEvent ();

signals:
    void eventSelected (QSharedPointer<SignalEvent const> selected_event);
    void signalHeightChanged (uint32 signal_height);
    void signalSpacingChanged (unsigned signal_spacing);
    void shownEventTypesChanged (std::set<uint16> shown_event_types);

protected:
    virtual void modeChanged (SignalVisualisationMode mode);

private:
    //-------------------------------------------------------------------------
    void addChannel (ChannelID channel_nr);

    //-------------------------------------------------------------------------
    void removeChannel (ChannelID channel_nr);

    //-------------------------------------------------------------------------
    static uint8 const NAVIGATION_Z = 1;
    static uint8 const X_GRID_Z = 2;
    static uint8 const CHANNEL_SEPARATOR_Z = 3;
    static uint8 const SIGNAL_Z = 4;
    static uint8 const EVENT_Z = 5;

    QSharedPointer<ChannelManager> channel_manager_;
    QSharedPointer<FileContext> file_context_;
    QSharedPointer<TabContext> tab_context_;
    SignalBrowserView* signal_browser_view_;

    typedef std::map<int32, SignalGraphicsItem*> Int2SignalGraphicsItemPtrMap;
    typedef std::map<int32, EventGraphicsItem*> Int2EventGraphicsItemPtrMap;

    typedef std::map<int32, int32> Int2IntMap;


    bool checkSignalBrowserPtr(const QString function);
    void updateEventItemsImpl ();

    // items
    Int2SignalGraphicsItemPtrMap channel2signal_item_;
    Int2EventGraphicsItemPtrMap id2event_item_;

    Int2IntMap channel2y_pos_;
    EventGraphicsItem* selected_event_item_;

    // parameters
    float64 pixel_per_sec_;

    int32 signal_height_;
    int32 signal_spacing_;
    int32 prefered_x_grid_pixel_intervall_;
    int32 prefered_y_grid_pixel_intervall_;
    float64 x_grid_pixel_intervall_;
    std::set<EventType> shown_event_types_;
    uint16 actual_event_creation_type_;

    bool show_y_grid_;
    bool show_x_grid_;
    ScaleMode auto_zoom_type_;
};

} // namespace PortingToQT4_

#endif

