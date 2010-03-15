// signal_browser_model.h
#ifndef SIGNAL_BROWSER_MODEL_H_q4
#define SIGNAL_BROWSER_MODEL_H_q4

#include "../base/file_signal_reader.h"
#include "../gui_signal_buffer.h"
#include "../abstract_browser_model.h"
#include "../tab_context.h"
#include "signal_browser_modes.h"
#include "event_graphics_item.h"

#include <QPointer>
#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QPointF>

#include <map>
#include <vector>
#include <set>

namespace BioSig_
{

class MainWindowModel;
class BasicHeader;
class FileSignalReader;
class SignalBrowserView;
class SignalGraphicsItem;
class EventTableFileReader;

// signal browser model
class SignalBrowserModel : public QObject, public AbstractBrowserModel
{
    Q_OBJECT

public:

    typedef FileSignalReader::SignalEventVector SignalEventVector;
    typedef QList<uint16> IntList;

    SignalBrowserModel(FileSignalReader& reader,
                       MainWindowModel& main_window_model,
                       QSharedPointer<EventTableFileReader const> event_table_file_reader,
                       TabContext& tab_context);
    virtual ~SignalBrowserModel();

    virtual void setPixelPerXUnit (float64 pixel_per_sec);
    virtual float64 getPixelPerXUnit () const;

    virtual void setItemsHeight (int32 height);


    virtual void updateLayout ();



    MainWindowModel& getMainWindowModel();
    void setSignalBrowserView(SignalBrowserView* signal_browser_view);
    void setLogStream(QTextStream* log_stream);
    QTextStream& getLogStream();
    void loadSettings();
    void saveSettings();
    void setMode(SignalBrowserMode mode);
    SignalBrowserMode getMode();

    void setWholeDataBuffer(SignalBuffer::WHOLE_BUFFER whole_buffer);
    void enableInitDownsampling(bool enabled);
    void enableInitMinMaxSearch(bool enabled);
    void setDefaultRange(float32 min, float32 max);


    //settings
    std::map<std::string, bool> getHideableWidgetsVisibilities () const;
    void setHideableWidgetsVisibilities (std::map<std::string, bool> const &widgets_visiblities);
    void setXGridVisible(bool visible);
    bool getGridVisible () const;
    void setYGridVisible(bool visible);
    bool isShowAllEventTypes() const;
    void setAutoZoomBehaviour (ScaleMode auto_zoom_type);
    ScaleMode getAutoZoomBehaviour () const;

    // buffer
    void initBuffer();
    SignalBuffer const& getSignalBuffer () const;
/*
    void setReleaseBuffer(bool release);
    bool getReleaseBuffer();
*/

    // channels
    void addChannel(uint32 channel_nr);
    void removeChannel(uint32 channel_nr);
    bool isChannelShown(uint32 channel_nr) const;
    std::map<uint32, QString> getShownChannels () const;
    uint32 getNumberShownChannels() const;
    int32 getYPosOfChannel (uint32 channel_nr) const;
/*
    int32 getChannelNr(uint32 shown_nr);
*/

    QPointF getViewingPosition ();
    void setViewingPosition (QPointF topleft);

    // actions
    void zoomInAll();
    void zoomOutAll();

    void autoScaleAll();

    void goTo(float32 sec, int32 channel_index);
    void goToAndSelectNextEvent (bool forward);
/*
    void zoomRect(const QRect& rect);
*/
/*
    void setSignalSpacing(int32 spacing);
    void setPreferedXGirdPixelIntervall(int32 pixel_intervall);
    void setPreferedYGirdPixelIntervall(int32 pixel_intervall);
*/
    // get parameters
    int32 getSignalHeight();
    int32 getSignalSpacing();
    int32 getVisibleWidth();
    /*    int32 getPreferedXGirdPixelIntervall();
*/
    int32 getPreferedYGirdPixelIntervall();
    float64 getXGridPixelIntervall();

    // events
    QString getEventName (uint16 event_type_id) const;
    QColor getEventColor (uint16 event_type_id) const;
    void getShownEventTypes(IntList& event_type);
    std::set<uint16> getShownEventTypes () const;
    std::set<uint16> getDisplayedEventTypes () const;
    void setShownEventTypes(const IntList& event_type, const bool all = false);
    void removeEvent(uint32 id, bool update = true);

    QSharedPointer<EventGraphicsItem> addEvent(QSharedPointer<SignalEvent> event, bool update = true);
    void addEvent(QSharedPointer<EventGraphicsItem> event);
    void unsetSelectedEventItem();
    QSharedPointer<EventGraphicsItem> getSelectedEventItem();
    QSharedPointer<SignalEvent> getSelectedSignalEvent();
    QSharedPointer<EventGraphicsItem> getEventItem(int32 id);
    void setSelectedEventToAllChannels();
    void changeSelectedEventChannel();
    void copySelectedEventToChannels();
    void changeSelectedEventType();
    void removeSelectedEvent();

    void getEvents(SignalEventVector& event_vector);
    uint16 getActualEventCreationType () const;

    void updateEventItems ();

public slots:
    void setEventChanged(int32 id);
    void setActualEventCreationType (uint16 new_type);
    void selectEvent (int32 id);
    void unselectEvent ();

signals:
    void eventSelected (QSharedPointer<SignalEvent> selected_event);
    void signalHeightChanged (unsigned signal_height);
    void signalSpacingChanged (unsigned signal_spacing);
    void shownEventTypesChanged (std::set<uint16> shown_event_types);

private:
    static uint8 const NAVIGATION_Z = 1;
    static uint8 const X_GRID_Z = 2;
    static uint8 const CHANNEL_SEPARATOR_Z = 3;
    static uint8 const SIGNAL_Z = 4;
    static uint8 const EVENT_Z = 5;

    enum State
    {
        STATE_READY,
        STATE_INIT_BUFFER
    };

    TabContext& tab_context_;
    SignalBrowserView* signal_browser_view_;
    QTextStream* log_stream_; // no auto_ptr
    MainWindowModel& main_window_model_;
    State state_;
    SignalBrowserMode mode_;



    typedef std::map<int32, SignalGraphicsItem*> Int2SignalGraphicsItemPtrMap;
    typedef QMap<int32, QSharedPointer<EventGraphicsItem> > Int2EventGraphicsItemPtrMap;

    typedef std::map<int32, int32> Int2IntMap;

    bool checkReadyState(const QString& function);
    bool checkSignalBrowserPtr(const QString function);
    void updateEventItemsImpl ();

    GUISignalBuffer signal_buffer_;


    QPointer<BasicHeader> basic_header_;

    // items
    Int2SignalGraphicsItemPtrMap channel2signal_item_;
    Int2EventGraphicsItemPtrMap id2event_item_;

    Int2IntMap channel2y_pos_;
    QSharedPointer<EventGraphicsItem> selected_event_item_;
    QSharedPointer<EventTableFileReader const> event_table_file_reader_;

    // parameters
    bool release_buffer_;
    float64 pixel_per_sec_;
    int32 signal_height_;
    int32 signal_spacing_;
    int32 prefered_x_grid_pixel_intervall_;
    int32 prefered_y_grid_pixel_intervall_;
    float64 x_grid_pixel_intervall_;
    std::set<uint16> shown_event_types_;
    uint16 actual_event_creation_type_;

    bool show_y_grid_;
    bool show_x_grid_;
    ScaleMode auto_zoom_type_;
    bool all_event_types_selected_;

};

} // namespace PortingToQT4_

#endif

