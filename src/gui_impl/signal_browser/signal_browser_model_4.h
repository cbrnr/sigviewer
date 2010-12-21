// signal_browser_model.h
#ifndef SIGNAL_BROWSER_MODEL_H_q4
#define SIGNAL_BROWSER_MODEL_H_q4

#include "tab_context.h"
#include "file_handling/channel_manager.h"
#include "file_handling/event_manager.h"
#include "gui/signal_visualisation_modes.h"
#include "gui/signal_visualisation_model.h"
#include "gui/color_manager.h"
#include "event_graphics_item.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QPointF>
#include <QList>

#include <map>
#include <set>

namespace SigViewer_
{

class SignalBrowserView;
class SignalGraphicsItem;

//-----------------------------------------------------------------------------
/// SignalBrowserModel
class SignalBrowserModel : public SignalVisualisationModel
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    SignalBrowserModel(QSharedPointer<EventManager> event_manager,
                       ChannelManager const& channel_manager,
                       QSharedPointer<TabContext> tab_context,
                       QSharedPointer<ColorManager const> color_manager);

    //-------------------------------------------------------------------------
    virtual ~SignalBrowserModel() {}

    //-------------------------------------------------------------------------
    virtual void scaleChannel (ChannelID id, float32 lower_value, float32 upper_value);

    //-------------------------------------------------------------------------
    virtual void scaleChannel (ChannelID id);

    //-------------------------------------------------------------------------
    virtual ChannelManager const& getChannelManager () const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager const> getEventManager () const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<EventManager> getEventManager ();

    //-------------------------------------------------------------------------
    /// see base class
    virtual unsigned getShownPosition () const;

    //-------------------------------------------------------------------------
    /// see base class
    virtual void goToSample (unsigned sample);

    //-------------------------------------------------------------------------
    /// see base class
    virtual QList<EventID> getSelectedEvents () const;

    //-------------------------------------------------------------------------
    virtual SignalVisualisationView const* view () const;

    void setSignalBrowserView(SignalBrowserView* signal_browser_view);
    void loadSettings();
    void saveSettings();

    virtual void setShownChannels (std::set<ChannelID> const& shown_channels);
    virtual std::set<ChannelID> getShownChannels () const;
    int32 getYPosOfChannel (uint32 channel_nr) const;

    void zoomInAll();
    void zoomOutAll();

    EventGraphicsItem* getSelectedEventItem();
    void updateEventItems ();

public slots:
    //-------------------------------------------------------------------------
    virtual void update ();

    //-------------------------------------------------------------------------
    /// adds the given event
    virtual void addEventItem (QSharedPointer<SignalEvent const> event);

    //-------------------------------------------------------------------------
    /// removes the given event
    virtual void removeEventItem (EventID id);

    void updateEvent (EventID id);

    virtual void selectEvent (EventID id);
    void unselectEvent ();

signals:
    void eventSelected (QSharedPointer<SignalEvent const> selected_event);

protected:
    virtual void shownEventTypesChangedImpl ();
    virtual void modeChangedImpl (SignalVisualisationMode mode);

private slots:
    //-------------------------------------------------------------------------
    /// implementation of removeEventItem which really deletes the item
    void removeEventItemImpl ();

private:

    //-------------------------------------------------------------------------
    void addChannel (ChannelID channel_nr);

    //-------------------------------------------------------------------------
    void removeChannel (ChannelID channel_nr);

    //-------------------------------------------------------------------------
    void updateEventItemsImpl ();

    //-------------------------------------------------------------------------
    static uint8 const SIGNAL_Z = 4;

    ChannelManager const& channel_manager_;
    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<TabContext> tab_context_;
    QSharedPointer<ColorManager const> color_manager_;
    SignalBrowserView* signal_browser_view_;

    typedef QMap<ChannelID, SignalGraphicsItem*> SignalGraphicsItemMap;
    typedef std::map<int32, EventGraphicsItem*> Int2EventGraphicsItemPtrMap;

    typedef std::map<int32, int32> Int2IntMap;


    SignalGraphicsItemMap channel2signal_item_;
    Int2EventGraphicsItemPtrMap id2event_item_;

    Int2IntMap channel2y_pos_;
    EventGraphicsItem* selected_event_item_;

    float64 x_grid_pixel_intervall_;

    bool show_y_grid_;
    bool show_x_grid_;
    bool initialized_;
    QList<EventGraphicsItem*> items_to_delete_;
};

} // namespace PortingToQT4_

#endif

