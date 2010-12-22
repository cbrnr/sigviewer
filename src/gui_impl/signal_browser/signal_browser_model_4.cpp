// signal_browser_model.cpp
#include "signal_browser_model_4.h"
#include "file_handling/event_manager.h"
#include "file_handling/channel_manager.h"
#include "signal_browser_view.h"
#include "signal_graphics_item.h"
#include "gui/color_manager.h"
#include "gui/progress_bar.h"
#include "base/signal_event.h"
#include "base/math_utils.h"
#include "gui_impl/gui_helper_functions.h"

#include <QTextStream>
#include <QApplication>
#include <QDialog>
#include <QInputDialog>
#include <QSettings>
#include <QDebug>

#include <cmath>
#include <algorithm>

namespace SigViewer_
{

namespace LayoutFunctions_
{
    unsigned getSceneHeight (unsigned num_channels, unsigned channel_height, float channel_overlapping)
    {
        if (num_channels == 0)
            return 0;

        return channel_height +
               (channel_height * (num_channels - 1) * (1.0 - channel_overlapping));
    }
}

//-----------------------------------------------------------------------------
SignalBrowserModel::SignalBrowserModel(QSharedPointer<EventManager> event_manager,
                                       ChannelManager const& channel_manager,
                                       QSharedPointer<TabContext> tab_context,
                                       QSharedPointer<ColorManager const> color_manager)
: SignalVisualisationModel (std::set<EventType> (),
                            channel_manager),
  channel_manager_ (channel_manager),
  event_manager_ (event_manager),
  tab_context_ (tab_context),
  color_manager_ (color_manager),
  signal_browser_view_ (0),
  selected_event_item_ (0),
  x_grid_pixel_intervall_(0),
  show_y_grid_(true),
  initialized_ (false)
{
    if (!event_manager_.isNull ())
    {
       QList<EventID> event_ids = event_manager_->getAllEvents();
       if (id2event_item_.size() == 0)
       {
           for (QList<EventID>::const_iterator event_id_iter = event_ids.begin ();
                event_id_iter != event_ids.end ();
                ++event_id_iter)
           {
               id2event_item_[*event_id_iter] = new EventGraphicsItem (
                       *this,
                       getSignalViewSettings(),
                       event_manager_->getEvent (*event_id_iter),
                       event_manager_,
                       tab_context_,
                       color_manager_);
           }
       }
    }
    connect (getSignalViewSettings().data(), SIGNAL(pixelsPerSampleChanged()), SLOT(update()));
    connect (getSignalViewSettings().data(), SIGNAL(channelHeightChanged()), SLOT(update()));
    connect (getSignalViewSettings().data(), SIGNAL(channelOverlappingChanged()), SLOT(update()));
}

//-----------------------------------------------------------------------------
SignalVisualisationView const* SignalBrowserModel::view () const
{
    return signal_browser_view_;
}


//-----------------------------------------------------------------------------
// set signal browser view
void SignalBrowserModel::setSignalBrowserView (SignalBrowserView* signal_browser_view)
{
    signal_browser_view_ = signal_browser_view;
    if (!event_manager_.isNull())
        setShownEventTypes (event_manager_->getEventTypes());

}

//-----------------------------------------------------------------------------
// load settings
void SignalBrowserModel::loadSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("SignalBrowserModel");
    show_x_grid_ = settings.value("show_x_grid", show_x_grid_).toBool();
    show_y_grid_ = settings.value("show_y_grid", show_y_grid_).toBool();

    settings.endGroup();
}

//-----------------------------------------------------------------------------
// save settings
void SignalBrowserModel::saveSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("SignalBrowserModel");
    settings.setValue("show_x_grid", show_x_grid_);
    settings.setValue("show_y_grid", show_y_grid_);

    settings.endGroup();
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::setShownChannels (std::set<ChannelID> const&
                                           new_shown_channels)
{
    unsigned new_channel_height = getSignalViewSettings()->getChannelHeight();
    if (!initialized_)
        new_channel_height = signal_browser_view_->getViewportHeight() / new_shown_channels.size();
    else
        new_channel_height *= static_cast<float>(getShownChannels().size()) / new_shown_channels.size();

    foreach (ChannelID channel, channel2signal_item_.keys())
        if (new_shown_channels.count (channel) == 0)
            removeChannel (channel);

    getSignalViewSettings()->setChannelHeight (std::max<unsigned>(20, new_channel_height));

    foreach (ChannelID channel, new_shown_channels)
    {
        ProgressBar::instance().increaseValue (1, tr("Creating view..."));
        if (channel2signal_item_.count (channel) == 0)
            addChannel (channel);
    }

    foreach (SignalGraphicsItem* channel, channel2signal_item_.values())
        channel->autoScale (getAutoScaleMode());

    if (!initialized_)
        goToSample (0);

    initialized_ = true;
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::addChannel (ChannelID channel_id)
{
    SignalGraphicsItem* signal_item
        = new SignalGraphicsItem (getSignalViewSettings(),
                                  event_manager_,
                                  tab_context_,
                                  channel_manager_,
                                  color_manager_,
                                  channel_id,
                                  *this);

    signal_item->connect (getSignalViewSettings().data(), SIGNAL(channelHeightChanged(uint)), SLOT(setHeight(uint)));
    channel2signal_item_[channel_id] = signal_item;
    signal_browser_view_->addSignalGraphicsItem (channel_id, signal_item, channel_manager_.getChannelLabel(channel_id));
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::removeChannel (ChannelID channel_id)
{
    SignalGraphicsItemMap::iterator sig_iter;

    sig_iter = channel2signal_item_.find (channel_id);
    if (sig_iter == channel2signal_item_.end())
         return;

    disconnect (sig_iter.value());

    signal_browser_view_->removeSignalGraphicsItem (channel_id, sig_iter.value());
    delete sig_iter.value();
    channel2signal_item_.erase (sig_iter);
}

//-----------------------------------------------------------------------------
std::set<ChannelID> SignalBrowserModel::getShownChannels () const
{
    std::set<ChannelID> shown_channels;
    for (SignalGraphicsItemMap::const_iterator sig_iter =
         channel2signal_item_.begin();
         sig_iter != channel2signal_item_.end();
         ++sig_iter)
    {
        shown_channels.insert(sig_iter.key());
    }
    return shown_channels;
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
// zoom in all
void SignalBrowserModel::zoomInAll()
{
    SignalGraphicsItemMap::iterator iter;
    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        iter.value()->zoomIn();
    }

    signal_browser_view_->updateWidgets();
}

//-----------------------------------------------------------------------------
// zoom out all
void SignalBrowserModel::zoomOutAll()
{
    SignalGraphicsItemMap::iterator iter;

    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        iter.value()->zoomOut();
    }

    signal_browser_view_->updateWidgets();
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::update()
{
    int32 width = channel_manager_.getNumberSamples()
                  * getSignalViewSettings()->getPixelsPerSample();

    int32 height = LayoutFunctions_::getSceneHeight (channel2signal_item_.size(),
                                                     getSignalViewSettings()->getChannelHeight(),
                                                     getSignalViewSettings()->getChannelOverlapping());

    signal_browser_view_->resizeScene (width, height);

    double pixel_per_sec = getSignalViewSettings()->getPixelsPerSample() * channel_manager_.getSampleRate();
    x_grid_pixel_intervall_ =  pixel_per_sec * MathUtils_::round125 (100.0 / pixel_per_sec);

    channel2y_pos_.clear();
    channel2y_pos_[UNDEFINED_CHANNEL] = 0;
    int32 y_pos = 0;
    SignalGraphicsItemMap::iterator signal_iter;

    for (signal_iter = channel2signal_item_.begin();
         signal_iter != channel2signal_item_.end();
         signal_iter++, y_pos += getSignalViewSettings()->getChannelHeight() * (1.0 - getSignalViewSettings()->getChannelOverlapping()))
    {
        channel2y_pos_[signal_iter.key()] = y_pos;
        signal_iter.value()->setHeight (getSignalViewSettings()->getChannelHeight());
        signal_iter.value()->setPos (0, y_pos);
        signal_iter.value()->setZValue(SIGNAL_Z);
        signal_iter.value()->updateYGridIntervall();

        signal_iter.value()->enableYGrid(show_y_grid_);
        signal_iter.value()->enableXGrid(show_x_grid_);
        signal_iter.value()->setXGridInterval (x_grid_pixel_intervall_);
        signal_iter.value()->show();
    }

    updateEventItemsImpl ();
    signal_browser_view_->update();
    signal_browser_view_->updateWidgets();
}

//-------------------------------------------------------------------------
void SignalBrowserModel::scaleChannel (ChannelID id, float32 lower_value, float32 upper_value)
{
    if (id == UNDEFINED_CHANNEL)
    {
        QProgressDialog progress;
        progress.setMaximum (channel2signal_item_.size());
        progress.setMinimum (0);
        progress.setModal (true);
        progress.setLabelText (tr("Scaling..."));
        progress.show ();
        for (SignalGraphicsItemMap::iterator it =
             channel2signal_item_.begin(); it != channel2signal_item_.end();
            ++it)
        {
            it.value()->scale (lower_value, upper_value);
            progress.setValue (progress.value() + 1);
        }
        progress.setValue (progress.maximum ());
    }
    else
        channel2signal_item_[id]->scale (lower_value, upper_value);
}

//-------------------------------------------------------------------------
void SignalBrowserModel::scaleChannel (ChannelID id)
{
    if (id == UNDEFINED_CHANNEL)
    {
        QProgressDialog progress;
        progress.setMaximum (channel2signal_item_.size());
        progress.setMinimum (0);
        progress.setModal (true);
        progress.setLabelText (tr("Scaling..."));
        progress.show ();
        for (SignalGraphicsItemMap::iterator it =
             channel2signal_item_.begin(); it != channel2signal_item_.end();
            ++it)
        {
            it.value()->autoScale (getAutoScaleMode());
            progress.setValue (progress.value() + 1);
        }
        progress.setValue (progress.maximum ());
    }
    else
        channel2signal_item_[id]->autoScale (getAutoScaleMode());

    signal_browser_view_->updateWidgets();
}

//-------------------------------------------------------------------------
ChannelManager const& SignalBrowserModel::getChannelManager () const
{
    return channel_manager_;
}

//-------------------------------------------------------------------------
QSharedPointer<EventManager const> SignalBrowserModel::getEventManager () const
{
    return event_manager_;
}

//-------------------------------------------------------------------------
QSharedPointer<EventManager> SignalBrowserModel::getEventManager ()
{
    return event_manager_;
}

//-------------------------------------------------------------------------
unsigned SignalBrowserModel::getShownPosition () const
{
    return signal_browser_view_->getVisibleX () / getSignalViewSettings()->getPixelsPerSample();
}

//-------------------------------------------------------------------------
void SignalBrowserModel::goToSample (unsigned sample)
{
    float32 position = 0;
    while (position < getSignalViewSettings()->getPixelsPerSample() * sample)
        position += getSignalViewSettings()->getPixelsPerSample();
    position -= getSignalViewSettings()->getPixelsPerSample();

    signal_browser_view_->goTo (position);
}

//-------------------------------------------------------------------------
QList<EventID> SignalBrowserModel::getSelectedEvents () const
{
    if (selected_event_item_)
        return QList<EventID> () << selected_event_item_->getId ();
    else
        return QList<EventID> ();
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
        selected_event_item_ = 0;
        emit eventSelected (QSharedPointer<SignalEvent const>(0));
        tab_context_->setSelectionState(TAB_STATE_NO_EVENT_SELECTED);
        return;
    }

    EventGraphicsItem* item = event_iter->second;

    if (selected_event_item_)
        selected_event_item_->setSelected (false);
    selected_event_item_ = item;
    selected_event_item_->setSelected (true);

    emit eventSelected(item->getSignalEvent());

    if (event_manager_->getEvent(id)->getChannel() ==
        UNDEFINED_CHANNEL)
        tab_context_->setSelectionState(TAB_STATE_EVENT_SELECTED_ALL_CHANNELS);
    else
        tab_context_->setSelectionState (TAB_STATE_EVENT_SELECTED_ONE_CHANNEL);
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
void SignalBrowserModel::shownEventTypesChangedImpl ()
{
    updateEventItemsImpl ();
}

//-------------------------------------------------------------------
void SignalBrowserModel::modeChangedImpl (SignalVisualisationMode mode)
{
    bool tooltips = (mode == MODE_INFO ||
                     mode == MODE_POINTER);
    foreach (SignalGraphicsItem* signal_item, channel2signal_item_.values())
        signal_item->setAcceptHoverEvents (tooltips);
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
                event_manager_->getEvent(event_iter->first);
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
            if (!(event_iter->second->scene()))
                signal_browser_view_->addEventGraphicsItem(event_iter->second);
            event_iter->second->show();
        }
    }
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::addEventItem (QSharedPointer<SignalEvent const> event)
{
    EventGraphicsItem* event_item = new EventGraphicsItem(
            *this, getSignalViewSettings(), event, event_manager_,
            tab_context_, color_manager_);

    id2event_item_[event->getId()] = event_item;
    signal_browser_view_->addEventGraphicsItem(event_item);
    event_item->updateToSignalEvent();
    event_item->show();
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::removeEventItem (EventID id)
{
    if (id2event_item_.find (id) == id2event_item_.end())
        return;

    EventGraphicsItem* event_item = id2event_item_[id];
    if (selected_event_item_ == event_item)
        unselectEvent ();
    id2event_item_.erase (id);
    items_to_delete_.push_back (event_item);
    GuiHelper::animateProperty (event_item, "opacity", 1.0, 0.0, this, SLOT(removeEventItemImpl()));
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::removeEventItemImpl ()
{
    EventGraphicsItem* event_item = items_to_delete_.front();
    items_to_delete_.pop_front();
    signal_browser_view_->removeEventGraphicsItem (event_item);
    delete event_item;
}

//-----------------------------------------------------------------------------
// set event changed
void SignalBrowserModel::updateEvent (EventID id)
{
    if (id2event_item_.find (id) == id2event_item_.end())
        return;

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

} // namespace SigViewer_
