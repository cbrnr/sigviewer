// signal_browser_model.cpp

#include "signal_browser_model_4.h"
#include "../file_handling/event_manager.h"
#include "../file_handling/channel_manager.h"
#include "signal_browser_view.h"
#include "signal_graphics_item.h"
#include "../editing_commands/change_channel_undo_command.h"
#include "../editing_commands/change_type_undo_command.h"
#include "../gui_impl/gui_helper_functions.h"
#include "../editing_commands/new_event_undo_command.h"

#include "../main_window_model_impl.h"
#include "../event_color_manager.h"
#include "../file_context.h"
#include "../base/math_utils.h"
#include "../base/signal_event.h"

#include <QTextStream>
#include <QApplication>
#include <QDialog>
#include <QInputDialog>
#include <QSettings>
#include <QProgressDialog>

#include <cmath>
#include <algorithm>

namespace BioSig_
{

//-----------------------------------------------------------------------------
// TODO! constructor
SignalBrowserModel::SignalBrowserModel(QSharedPointer<FileContext> file_context,
                                       QSharedPointer<TabContext> tab_context)
: AbstractBrowserModel (file_context->getChannelManager()->getSampleRate(),
                        file_context->getEventManager()->getAllPossibleEventTypes()),
  channel_manager_ (file_context->getChannelManager ()),
  file_context_ (file_context),
  tab_context_ (tab_context),
  signal_browser_view_ (0),
  selected_event_item_ (0),
  signal_height_(75),
  signal_spacing_(1),
  prefered_x_grid_pixel_intervall_(100),
  prefered_y_grid_pixel_intervall_(25),
  x_grid_pixel_intervall_(0),
  show_y_grid_(true)
{
    QList<EventID> event_ids = file_context_->getEventManager()->getAllEvents();
    if (id2event_item_.size() == 0)
    {
        for (QList<EventID>::const_iterator event_id_iter = event_ids.begin ();
             event_id_iter != event_ids.end ();
             ++event_id_iter)
        {
            id2event_item_[*event_id_iter] = new EventGraphicsItem (
                    *this,
                    file_context_->getEventManager()->getEvent (*event_id_iter),
                    file_context_->getEventManager(),
                    tab_context_);
        }
    }
}

//-----------------------------------------------------------------------------
// destructor
SignalBrowserModel::~SignalBrowserModel()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
// set signal browser view
void SignalBrowserModel::setSignalBrowserView(SignalBrowserView* signal_browser_view)
{
    signal_browser_view_ = signal_browser_view;
}

//-----------------------------------------------------------------------------
// load settings
void SignalBrowserModel::loadSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("SignalBrowserModel");

    signal_height_ = settings.value("signal_height", signal_height_).toInt();
    signal_spacing_ = settings.value("signal_spacing", signal_spacing_).toInt();
    prefered_x_grid_pixel_intervall_ = settings.value("prefered_x_grid_pixel_intervall",
                                                      prefered_x_grid_pixel_intervall_).toInt();
    prefered_y_grid_pixel_intervall_ = settings.value("prefered_y_grid_pixel_intervall",
                                                      prefered_y_grid_pixel_intervall_).toInt();
    show_x_grid_ = settings.value("show_x_grid", show_x_grid_).toBool();
    show_y_grid_ = settings.value("show_y_grid", show_y_grid_).toBool();
    auto_zoom_type_ = static_cast<ScaleMode>(settings.value("auto_zoom_type_", auto_zoom_type_).toUInt());

    settings.endGroup();
}

//-----------------------------------------------------------------------------
// save settings
void SignalBrowserModel::saveSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("SignalBrowserModel");

    settings.setValue("signal_height", signal_height_);
    settings.setValue("signal_spacing", signal_spacing_);
    settings.setValue("prefered_x_grid_pixel_intervall", prefered_x_grid_pixel_intervall_);
    settings.setValue("prefered_y_grid_pixel_intervall", prefered_y_grid_pixel_intervall_);
    settings.setValue("show_x_grid", show_x_grid_);
    settings.setValue("show_y_grid", show_y_grid_);
    settings.setValue("auto_zoom_type_", auto_zoom_type_);

    settings.endGroup();
}

//-----------------------------------------------------------------------------
bool SignalBrowserModel::setShownChannels (std::set<ChannelID> const&
                                           new_shown_channels)
{
    bool new_channels = true;
    if (new_shown_channels.size() == channel2signal_item_.size())
    {
        new_channels = false;
        for (std::set<ChannelID>::const_iterator channel = new_shown_channels.begin();
             channel != new_shown_channels.end();
             ++channel)
        {
            if (channel2signal_item_.count (*channel) == 0)
            {
                channel = new_shown_channels.end ();
                new_channels = true;
            }

        }
    }
    if (!new_channels)
        return new_channels;

    signal_height_ = signal_browser_view_->getVisibleHeight() /
                     new_shown_channels.size();
    signal_height_ -= signal_spacing_;

    for (Int2SignalGraphicsItemPtrMap::const_iterator channel = channel2signal_item_.begin();
         channel != channel2signal_item_.end();
         ++channel)
    {
        if (new_shown_channels.count (channel->first) == 0)
            removeChannel (channel->first);
    }

    QProgressDialog progress;
    progress.setMaximum (new_shown_channels.size());
    progress.setMinimum (0);
    progress.setModal (true);
    progress.setLabelText (tr("Buffering channel "));
    progress.show ();

    for (std::set<ChannelID>::const_iterator channel = new_shown_channels.begin();
         channel != new_shown_channels.end();
         ++channel)
    {
        progress.setValue (progress.value()+1);
        if (channel2signal_item_.count (*channel) == 0)
            addChannel (*channel);
    }
    progress.setValue (progress.maximum() );

    emit signalHeightChanged (signal_height_);

    for (std::set<ChannelID>::const_iterator channel = new_shown_channels.begin();
         channel != new_shown_channels.end();
         ++channel)
        channel2signal_item_[*channel]->autoScale (auto_zoom_type_);

    return new_channels;
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::addChannel (ChannelID channel_id)
{
    if (channel_id >= file_context_->getChannelManager()->getNumberChannels())
        return;

    SignalGraphicsItem* signal_item
        = new SignalGraphicsItem (file_context_->getEventManager(),
                                  tab_context_,
                                  file_context_->getChannelManager(),
                                  channel_id,
                                  file_context_->getChannelManager()->getSignalChannel(channel_id),
                                  *this);

    signal_item->connect (this, SIGNAL(signalHeightChanged(uint32)), SLOT(setHeight(uint32)));
    channel2signal_item_[channel_id] = signal_item;
    signal_browser_view_->addSignalGraphicsItem (channel_id, signal_item);
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::removeChannel (ChannelID channel_id)
{
    Int2SignalGraphicsItemPtrMap::iterator sig_iter;

    sig_iter = channel2signal_item_.find (channel_id);
    if (sig_iter == channel2signal_item_.end())
         return;

    disconnect (sig_iter->second);

    signal_browser_view_->removeSignalGraphicsItem (channel_id, sig_iter->second);
    channel2signal_item_.erase (sig_iter);
    delete sig_iter->second;
}

//-----------------------------------------------------------------------------
bool SignalBrowserModel::isChannelShown(uint32 channel_nr) const
{
    return channel2signal_item_.find(channel_nr) != channel2signal_item_.end();
}

//-----------------------------------------------------------------------------
std::set<ChannelID> SignalBrowserModel::getShownChannels () const
{
    std::set<ChannelID> shown_channels;
    for (Int2SignalGraphicsItemPtrMap::const_iterator sig_iter =
         channel2signal_item_.begin();
         sig_iter != channel2signal_item_.end();
         ++sig_iter)
    {
        shown_channels.insert(sig_iter->first);
    }
    return shown_channels;
}


//-----------------------------------------------------------------------------
std::map<uint32, QString> SignalBrowserModel::getShownChannelsWithLabels () const
{
    std::map<uint32, QString> shown_channels;
    for (Int2SignalGraphicsItemPtrMap::const_iterator sig_iter =
         channel2signal_item_.begin();
         sig_iter != channel2signal_item_.end();
         ++sig_iter)
    {
        shown_channels[sig_iter->first] = sig_iter->second->getLabel();
    }
    return shown_channels;
}

//-----------------------------------------------------------------------------
// get number shown channels
uint32 SignalBrowserModel::getNumberShownChannels() const
{
    return channel2signal_item_.size();
}

//-----------------------------------------------------------------------------
int32 SignalBrowserModel::getYPosOfChannel (uint32 channel_nr) const
{
    Int2IntMap::const_iterator y_iter =
             channel2y_pos_.find(channel_nr);
    if (y_iter != channel2y_pos_.end())
        return y_iter->second;
    else
        return 0;
}

//-----------------------------------------------------------------------------
// get viewing position
QPointF SignalBrowserModel::getViewingPosition ()
{
    return QPointF(static_cast<float>(signal_browser_view_->getVisibleX()) / getPixelPerSample (), signal_browser_view_->getVisibleY());
}

//-----------------------------------------------------------------------------
// set viewing position
void SignalBrowserModel::setViewingPosition (QPointF topleft)
{
    signal_browser_view_->goTo(topleft.x() * getPixelPerSample (), topleft.y());
}



//-----------------------------------------------------------------------------
// zoom in all
void SignalBrowserModel::zoomInAll()
{
    Int2SignalGraphicsItemPtrMap::iterator iter;
    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        iter->second->zoomIn();
    }

    signal_browser_view_->updateWidgets();
}

//-----------------------------------------------------------------------------
// zoom out all
void SignalBrowserModel::zoomOutAll()
{
    Int2SignalGraphicsItemPtrMap::iterator iter;

    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        iter->second->zoomOut();
    }

    signal_browser_view_->updateWidgets();
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::autoScaleAll()
{
    Int2SignalGraphicsItemPtrMap::iterator iter;

    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        iter->second->autoScale(auto_zoom_type_);
    }

    signal_browser_view_->updateWidgets();
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::setPixelPerXUnit(float64 pixel_per_sec)
{
    pixel_per_sec_ = pixel_per_sec;
}

// get pixel per sec
float64 SignalBrowserModel::getPixelPerXUnit() const
{
    return pixel_per_sec_;
}



// set signal height
void SignalBrowserModel::setItemsHeight(int32 height)
{
    signal_height_ = height;
    emit signalHeightChanged (height);
}

// get signal height
int32 SignalBrowserModel::getSignalHeight()
{
    return signal_height_;
}

// update layout
void SignalBrowserModel::updateLayout()
{
    int32 width = file_context_->getChannelManager()->getNumberSamples()
                  * getPixelPerSample();

    int32 height = (signal_height_  + signal_spacing_) *
                   channel2signal_item_.size();

    signal_browser_view_->resizeScene (width, height);

    // singanls position
    channel2y_pos_.clear();
    channel2y_pos_[UNDEFINED_CHANNEL] = 0;
    int32 y_pos = 0;
    Int2SignalGraphicsItemPtrMap::iterator signal_iter;

    for (signal_iter = channel2signal_item_.begin();
         signal_iter != channel2signal_item_.end();
         signal_iter++, y_pos += signal_height_ + signal_spacing_)
    {
        channel2y_pos_[signal_iter->first] = y_pos;
        signal_iter->second->setHeight (signal_height_);
        signal_iter->second->setPos (0, y_pos);
        signal_iter->second->setZValue(SIGNAL_Z);
        signal_iter->second->updateYGridIntervall();

        signal_iter->second->enableYGrid(show_y_grid_);
        signal_iter->second->enableXGrid(show_x_grid_);
        signal_iter->second->show();
    }

    updateEventItemsImpl ();

    // update x grid intervall
    float64 pixel_per_sec = getPixelPerSample () * channel_manager_->getSampleRate();
    float64 x_grid_intervall = round125(prefered_x_grid_pixel_intervall_ /
                                        pixel_per_sec);

    x_grid_pixel_intervall_ =  pixel_per_sec * x_grid_intervall;
    signal_browser_view_->setXAxisIntervall (x_grid_pixel_intervall_);
    emit pixelPerSampleChanged (getPixelPerSample (), getSampleRate());
    signal_browser_view_->update();
    signal_browser_view_->updateWidgets();
}

//-------------------------------------------------------------------
void SignalBrowserModel::zoom (ZoomDimension dimension, float factor)
{
    if (dimension == ZOOM_VERTICAL || dimension == ZOOM_BOTH)
    {
        if (factor > 0)
        {
            signal_height_ *= factor;
        }
        else
        {
            signal_height_ /= -factor;
        }
        emit signalHeightChanged (signal_height_);
    }

    updateLayout ();
}

//-------------------------------------------------------------------------
unsigned SignalBrowserModel::getShownSignalWidth () const
{
    return signal_browser_view_->getVisibleWidth ();
}

//-------------------------------------------------------------------------
unsigned SignalBrowserModel::getShownPosition () const
{
    return signal_browser_view_->getVisibleX () / getPixelPerSample ();
}

//-------------------------------------------------------------------------
void SignalBrowserModel::goToSample (unsigned sample)
{
    float32 position = 0;
    while (position < getPixelPerSample() * sample)
        position += getPixelPerSample ();
    position -= getPixelPerSample ();

    signal_browser_view_->goTo (position, 0);
}

//-------------------------------------------------------------------------
EventID SignalBrowserModel::getSelectedEvent () const
{
    if (selected_event_item_)
        return selected_event_item_->getId ();
    else
        return UNDEFINED_EVENT_ID;
}

//-------------------------------------------------------------------
void SignalBrowserModel::updateEventItems ()
{
    updateEventItemsImpl ();
    signal_browser_view_->update ();
}

//-------------------------------------------------------------------
void SignalBrowserModel::selectEvent (EventID id)
{
    Int2EventGraphicsItemPtrMap::iterator event_iter = id2event_item_.find (id);
    if (event_iter == id2event_item_.end())
    {
        tab_context_->setSelectionState(TAB_STATE_NO_EVENT_SELECTED);
        selected_event_item_ = 0;
        emit eventSelected (QSharedPointer<SignalEvent const>(0));
        return;
    }

    EventGraphicsItem* item = event_iter->second;

    if (file_context_->getEventManager()->getEvent(id)->getChannel() ==
        UNDEFINED_CHANNEL)
        tab_context_->setSelectionState(TAB_STATE_EVENT_SELECTED_ALL_CHANNELS);
    else
        tab_context_->setSelectionState (TAB_STATE_EVENT_SELECTED_ONE_CHANNEL);

    if (selected_event_item_)
        selected_event_item_->setSelected (false);
    selected_event_item_ = item;
    selected_event_item_->setSelected (true);

    emit eventSelected(item->getSignalEvent());
}

//-------------------------------------------------------------------
void SignalBrowserModel::unselectEvent ()
{    
    tab_context_->setSelectionState (TAB_STATE_NO_EVENT_SELECTED);
    if (selected_event_item_)
        selected_event_item_->setSelected (false);
    selected_event_item_ = 0;
    emit eventSelected (QSharedPointer<SignalEvent const>(0));
}

//-------------------------------------------------------------------
void SignalBrowserModel::modeChanged (SignalVisualisationMode mode)
{
    if (mode == MODE_HAND)
        signal_browser_view_->setScrollMode (true);
    else
        signal_browser_view_->setScrollMode (false);
}

//-------------------------------------------------------------------
void SignalBrowserModel::shownEventTypesChangedImpl ()
{
    updateEventItemsImpl ();
}


// get signal spacing
int32 SignalBrowserModel::getSignalSpacing()
{
    return signal_spacing_;
}

//-------------------------------------------------------------------
int32 SignalBrowserModel::getVisibleWidth()
{
    return signal_browser_view_->getVisibleWidth();
}

// get prefered y grid pixel intervall
int32 SignalBrowserModel::getPreferedYGirdPixelIntervall()
{
    return prefered_y_grid_pixel_intervall_;
}

// get x grid pixel intervall
float64 SignalBrowserModel::getXGridPixelIntervall()
{
    return x_grid_pixel_intervall_;
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::updateEventItemsImpl ()
{
    Int2EventGraphicsItemPtrMap::iterator event_iter;
    for (event_iter = id2event_item_.begin();
         event_iter != id2event_item_.end();
         event_iter++)
    {
        QSharedPointer<SignalEvent const> event =
                file_context_->getEventManager()->getEvent(event_iter->first);
        if (!event)
        {
            continue;
        }

        Int2IntMap::iterator y_pos_iter;
        y_pos_iter = channel2y_pos_.find(event->getChannel());

        std::set<EventType> shown_event_types = getShownEventTypes ();
        if (!shown_event_types.count(event->getType()) ||
            y_pos_iter == channel2y_pos_.end())
        {
            event_iter->second->hide();
            if (event_iter->second == selected_event_item_)
            {
                selected_event_item_->setSelected(false);
                selected_event_item_ = 0;
                tab_context_->setSelectionState (TAB_STATE_NO_EVENT_SELECTED);
            }
        }
        else
        {
            event_iter->second->updateToSignalEvent ();
            signal_browser_view_->addEventGraphicsItem(event_iter->second);
            event_iter->second->show();
        }
    }
}


//-----------------------------------------------------------------------------
// goTo
void SignalBrowserModel::goTo(float32 sec, int32 channel_index)
{
    float32 x = sec * getPixelPerSample() / file_context_->getChannelManager()->getSampleRate();
    float32 y = channel_index < 0 ? signal_browser_view_->getVisibleY()
                                : channel_index *
                                  (signal_height_ + signal_spacing_);
    signal_browser_view_->goTo(x, y);
    //signal_browser_view_->smoothGoTo (x, y);
}

//-----------------------------------------------------------------------------
std::set<uint16> SignalBrowserModel::getDisplayedEventTypes () const
{
    std::set<uint16> present_event_types;
    for (Int2EventGraphicsItemPtrMap::const_iterator event_it =
         id2event_item_.begin();
         event_it != id2event_item_.end();
         ++event_it)
    {
        if (event_it->second->isVisible())
            present_event_types.insert (event_it->second->getSignalEvent()->getType());
    }
    return present_event_types;
}


//-----------------------------------------------------------------------------
void SignalBrowserModel::setShownEventTypes(const IntList& event_type)
{
    assert (0);
}


//-----------------------------------------------------------------------------
void SignalBrowserModel::addEventItem (QSharedPointer<SignalEvent const> event)
{
    EventGraphicsItem* event_item = new EventGraphicsItem(
            *this, event, file_context_->getEventManager(),
            tab_context_);

    id2event_item_[event->getId()] = event_item;
    signal_browser_view_->addEventGraphicsItem(event_item);
    event_item->updateToSignalEvent();
    event_item->show();
    file_context_->setState (FILE_STATE_CHANGED);
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::removeEventItem (EventID id)
{
    if (id2event_item_.find (id) == id2event_item_.end())
        return;

    file_context_->setState (FILE_STATE_CHANGED);
    EventGraphicsItem* event_item = id2event_item_[id];
    id2event_item_.erase (id);
    signal_browser_view_->removeEventGraphicsItem (event_item);
    unselectEvent ();
    delete event_item;
}


//-----------------------------------------------------------------------------
// set event changed
void SignalBrowserModel::setEventChanged (EventID id)
{
    if (id2event_item_.find (id) == id2event_item_.end())
        return;

    file_context_->setState (FILE_STATE_CHANGED);
    EventGraphicsItem* event_item = id2event_item_[id];
    event_item->updateToSignalEvent ();
    if (selected_event_item_)
        if (selected_event_item_->getId() == id)
        {
            if (selected_event_item_->getSignalEvent()->getChannel() ==
                UNDEFINED_CHANNEL)
                tab_context_->setSelectionState (TAB_STATE_EVENT_SELECTED_ALL_CHANNELS);
            else
                tab_context_->setSelectionState (TAB_STATE_EVENT_SELECTED_ONE_CHANNEL);

            emit eventSelected (event_item->getSignalEvent());
        }
}

//-----------------------------------------------------------------------------
// get selected event item
EventGraphicsItem* SignalBrowserModel::getSelectedEventItem()
{
    return selected_event_item_;
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> SignalBrowserModel::getSelectedSignalEvent()
{
    return selected_event_item_->getSignalEvent();
}

//-------------------------------------------------------------------------
std::map<std::string, bool> SignalBrowserModel::getHideableWidgetsVisibilities () const
{
    return signal_browser_view_->getWidgetVisibilities();
}

//-------------------------------------------------------------------------
void SignalBrowserModel::setHideableWidgetsVisibilities (std::map<std::string, bool> const &widgets_visiblities)
{
    for (std::map<std::string, bool>::const_iterator widget_iterator = widgets_visiblities.begin();
         widget_iterator != widgets_visiblities.end();
         ++widget_iterator)
    {
        signal_browser_view_->setWidgetVisibility(widget_iterator->first, widget_iterator->second);
    }
}

//-------------------------------------------------------------------------
void SignalBrowserModel::setXGridVisible(bool visible)
{
    show_x_grid_ = visible;
}

//-------------------------------------------------------------------------
bool SignalBrowserModel::getGridVisible () const
{
    return show_x_grid_ || show_y_grid_;
}


//-------------------------------------------------------------------------
void SignalBrowserModel::setYGridVisible(bool visible)
{
    show_y_grid_ = visible;
}

//-------------------------------------------------------------------------
void SignalBrowserModel::setAutoZoomBehaviour(ScaleMode auto_zoom_type)
{
    auto_zoom_type_ = auto_zoom_type;
}

//-------------------------------------------------------------------------
ScaleMode SignalBrowserModel::getAutoZoomBehaviour () const
{
    return auto_zoom_type_;
}

} // namespace BioSig_
