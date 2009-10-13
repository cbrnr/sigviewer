// signal_browser_model.h
#ifndef SIGNAL_BROWSER_MODEL_H_q4
#define SIGNAL_BROWSER_MODEL_H_q4

#include "../base/file_signal_reader.h"
#include "../gui_signal_buffer.h"

/*
#include <QColor>
#include <QMap>


class QRect;
class QTextStream;
*/

#include <QPointer>
#include <QObject>
#include <QMap>
#include <QSharedPointer>

#include <map>

namespace BioSig_
{

class MainWindowModel;
class BasicHeader;
class FileSignalReader;

namespace PortingToQT4_
{

class SignalBrowserView;
class SignalGraphicsItem;
class EventGraphicsItem;

// signal browser model
class SignalBrowserModel : public QObject
{
    Q_OBJECT

public:

    typedef FileSignalReader::SignalEventVector SignalEventVector;
    typedef QList<uint16> IntList;


    enum Mode
    {
        MODE_NEW,
        MODE_POINTER,
        MODE_HAND,
        MODE_SHIFT_SIGNAL,
        MODE_ZOOM
    };

    SignalBrowserModel(FileSignalReader& reader,
                       MainWindowModel& main_window_model);
    virtual ~SignalBrowserModel();

    MainWindowModel& getMainWindowModel();
    void setSignalBrowserView(SignalBrowserView* signal_browser_view);
    void setLogStream(QTextStream* log_stream);
    QTextStream& getLogStream();
    void loadSettings();
    void saveSettings();
    void setMode(Mode mode);
    Mode getMode();

    void setWholeDataBuffer(SignalBuffer::WHOLE_BUFFER whole_buffer);
    void enableInitDownsampling(bool enabled);
    void enableInitMinMaxSearch(bool enabled);
    void setDefaultRange(float32 min, float32 max);


    //settings
    void showXScales(bool enabled);
    void showYScales(bool enabled);
    void showChannelLabels(bool enabled);
    void setXGridVisible(bool visible);
    void setYGridVisible(bool visible);

    bool isShowAllEventTypes() const;

    void setAutoZoomBehaviour(ScaleMode auto_zoom_type);

    // buffer
    void initBuffer();
/*
    void setReleaseBuffer(bool release);
    bool getReleaseBuffer();
*/

    // channels
    void addChannel(uint32 channel_nr);
    void removeChannel(uint32 channel_nr);
    bool isChannelShown(uint32 channel_nr);
    uint32 getNumberShownChannels();
/*
    int32 getChannelNr(uint32 shown_nr);
*/
    // actions
    void zoomInAll();
    void zoomOutAll();

    void autoScaleAll();

    void goTo(int32 sec, int32 channel_index);
    void goToAndSelectNextEvent ();
/*
    void zoomRect(const QRect& rect);
*/
    // set parameters
    void setPixelPerSec(float64 pixel_per_sec);
    void setSignalHeight(int32 height);
/*
    void setSignalSpacing(int32 spacing);
    void setPreferedXGirdPixelIntervall(int32 pixel_intervall);
    void setPreferedYGirdPixelIntervall(int32 pixel_intervall);
*/
    // get parameters
    float64 getPixelPerSec();
    int32 getSignalHeight();
    int32 getSignalSpacing();
/*    int32 getPreferedXGirdPixelIntervall();
*/
    int32 getPreferedYGirdPixelIntervall();
    float64 getXGridPixelIntervall();

    // events
    void getShownEventTypes(IntList& event_type);
    void setShownEventTypes(const IntList& event_type, const bool all);
    void setEventChanged(uint32 id, bool update = true);
    void removeEvent(uint32 id, bool update = true);

    /// TODO QT4: also some addEvent-method is needed which returns the
    ///           newly created EventGraphicsItem!!!
    QSharedPointer<EventGraphicsItem> addEvent(QSharedPointer<SignalEvent> event, bool update = true);
    void addEvent(QSharedPointer<EventGraphicsItem> event, bool update = true);
    void setSelectedEventItem(QSharedPointer<EventGraphicsItem> item);
    void unsetSelectedEventItem();
    QSharedPointer<EventGraphicsItem> getSelectedEventItem();
    QSharedPointer<EventGraphicsItem> getEventItem(int32 id);
    void setSelectedEventToAllChannels();
    void changeSelectedEventChannel();
    void copySelectedEventToChannels();
    void changeSelectedEventType();
    void removeSelectedEvent();

    void getEvents(SignalEventVector& event_vector);
    uint16 getActualEventCreationType () const;

    void updateLayout();
/*
public slots:

    void canvasViewContentMoving(int content_x, int content_y);
*/
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

    SignalBrowserView* signal_browser_view_;
    QTextStream* log_stream_; // no auto_ptr
    MainWindowModel& main_window_model_;
    State state_;
    Mode mode_;



    typedef std::map<int32, SignalGraphicsItem*> Int2SignalGraphicsItemPtrMap;
    typedef QMap<int32, QSharedPointer<EventGraphicsItem> > Int2EventGraphicsItemPtrMap;

    typedef std::map<int32, int32> Int2IntMap;

    void resetEventSizeAndPos (QSharedPointer<EventGraphicsItem> event);
    bool checkReadyState(const QString& function);
    bool checkSignalBrowserPtr(const QString function);

    GUISignalBuffer signal_buffer_;


    QPointer<BasicHeader> basic_header_;

    // items
    Int2SignalGraphicsItemPtrMap channel2signal_item_;
/*
    XGridCanvasItem* x_grid_item_;
    ChannelSeparatorCanvasItem* channel_separator_item_;
    NavigationCanvasItem* navigation_item_;
*/
    Int2EventGraphicsItemPtrMap id2event_item_;

    Int2IntMap channel2y_pos_;
    QSharedPointer<EventGraphicsItem> selected_event_item_;

    // parameters
    bool release_buffer_;
    float64 pixel_per_sec_;
    int32 signal_height_;
    int32 signal_spacing_;
    int32 prefered_x_grid_pixel_intervall_;
    int32 prefered_y_grid_pixel_intervall_;
    float64 x_grid_pixel_intervall_;
    IntList shown_event_types_;
    uint16 actual_event_creation_type_;

    bool show_y_grid_;

    bool show_x_grid_;
    bool show_channel_labels_;
    bool show_y_scales_;
    bool show_x_scales_;
    ScaleMode auto_zoom_type_;
    bool all_event_types_selected_;

};

} // namespace PortingToQT4_

} // namespace Biosig_

#endif

