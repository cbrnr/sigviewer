// signal_browser_model.h
#ifndef SIGNAL_BROWSER_MODEL_H
#define SIGNAL_BROWSER_MODEL_H

#include "gui_signal_buffer.h"
#include "base/file_signal_reader.h"
#include "settings_dialog.h"

#include <QObject>
#include <QColor>
#include <QMap>
#include <QList>
#include <QPointer>

class QRect;
class QTextStream;

namespace BioSig_
{

class MainWindowModel;
class SignalBrowser;
class BasicHeader;
class FileSignalReader;
class SignalCanvasItem;
class XGridCanvasItem;
class ChannelSeparatorCanvasItem;
class NavigationCanvasItem;
class EventCanvasItem;

// signal browser model
class SignalBrowserModel : public QObject
{
    Q_OBJECT

public:

    typedef FileSignalReader::SignalEventVector SignalEventVector;

    enum State
    {
        STATE_READY,
        STATE_INIT_BUFFER
    };

    enum Mode
    {
        MODE_NEW,
        MODE_POINTER,
        MODE_HAND,
        MODE_SHIFT_SIGNAL,
        MODE_ZOOM
    };

    enum ZOrder
    {
        NAVIGATION_Z = 1,
        X_GRID_Z = 2,
        CHANNEL_SEPARATOR_Z = 3,
        SIGNAL_Z = 4,
        EVENT_Z = 5
    };

    typedef QList<uint16> IntList;

    SignalBrowserModel(FileSignalReader& reader,
                       MainWindowModel& main_window_model);
    ~SignalBrowserModel();
    MainWindowModel& getMainWindowModel();
    void setSignalBrowser(SignalBrowser* browser);
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
    void setReleaseBuffer(bool release);
    bool getReleaseBuffer();

    // channels
    void addChannel(uint32 channel_nr);
    void removeChannel(uint32 channel_nr);
    bool isChannelShown(uint32 channel_nr);
    uint32 getNumberShownChannels();
    int32 getChannelNr(uint32 shown_nr);

    // actions
    void zoomInAll();
    void zoomOutAll();
    void autoScaleAll();
    void goTo(int32 sec, int32 channel_index);
    void zoomRect(const QRect& rect);

    // set parameters
    void setPixelPerSec(float64 pixel_per_sec);
    void setSignalHeight(int32 height);
    void setSignalSpacing(int32 spacing);
    void setPreferedXGirdPixelIntervall(int32 pixel_intervall);
    void setPreferedYGirdPixelIntervall(int32 pixel_intervall);

    // get parameters
    float64 getPixelPerSec();
    int32 getSignalHeight();
    int32 getSignalSpacing();
    int32 getPreferedXGirdPixelIntervall();
    int32 getPreferedYGirdPixelIntervall();
    float64 getXGridPixelIntervall();

    // events
    void getShownEventTypes(IntList& event_type);
    void setShownEventTypes(const IntList& event_type, const bool all);
    void setEventChanged(uint32 id, bool update = true);
    void removeEvent(uint32 id, bool update = true);
    EventCanvasItem* addEvent(const SignalEvent& event, bool update = true);
    void setSelectedEventItem(EventCanvasItem* item);
    EventCanvasItem* getSelectedEventItem();
    void setSelectedEventToAllChannels();
    void changeSelectedEventChannel();
    void copySelectedEventToChannels();
    void changeSelectedEventType();
    void removeSelectedEvent();
    void getEvents(SignalEventVector& event_vector);
    uint16 getActualEventCreationType () const;
    void updateLayout();

public slots:

    void canvasViewContentMoving(int content_x, int content_y);

private:

    typedef QMap<int32, SignalCanvasItem*> Int2SignalCanvasItemPtrMap;
    typedef QMap<int32, EventCanvasItem*> Int2EventCanvasItemPtrMap;
    typedef QMap<int32, int32> Int2IntMap;

    void setState(State state);
    bool checkReadyState(const QString& function);
    bool checkSignalBrowserPtr(const QString function);

    SignalBrowser* signal_browser_;
    QPointer<BasicHeader> basic_header_;
    GUISignalBuffer signal_buffer_;
    QTextStream* log_stream_; // no auto_ptr
    MainWindowModel& main_window_model_;
    State state_;
    Mode mode_;

    // items
    Int2SignalCanvasItemPtrMap channel2signal_item_;
    XGridCanvasItem* x_grid_item_;
    ChannelSeparatorCanvasItem* channel_separator_item_;
    NavigationCanvasItem* navigation_item_;
    Int2EventCanvasItemPtrMap id2event_item_;
    Int2IntMap channel2y_pos_;
    EventCanvasItem* selected_event_item_;

    // parameters
    bool release_buffer_;
    float64 pixel_per_sec_;
    int32 signal_height_;
    int32 signal_spacing_;
    int32 prefered_x_grid_pixel_intervall_;
    float64 x_grid_pixel_intervall_;
    int32 prefered_y_grid_pixel_intervall_;
    IntList shown_event_types_;
    uint16 actual_event_creation_type_;

    bool show_x_grid_;
    bool show_y_grid_;
    bool show_channel_labels_;
    bool show_y_scales_;
    bool show_x_scales_;
    ScaleMode auto_zoom_type_;
    bool all_event_types_selected_;
};

} // namespace Biosig_

#endif

