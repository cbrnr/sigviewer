// signal_browser_model.cpp

#include "signal_browser_model_4.h"
#include "event_manager_interface.h"
#include "channel_manager_interface.h"
#include "signal_browser_view.h"
#include "signal_graphics_item.h"
#include "event_graphics_item.h"
#include "change_channel_undo_command.h"
#include "change_type_undo_command.h"
#include "new_event_undo_command.h"

#include "../main_window_model.h"
#include "../event_color_manager.h"
#include "../file_context.h"
#include "../base/file_signal_reader.h"
#include "../base/math_utils.h"
#include "../base/signal_event.h"
#include "../base/event_table_file_reader.h"
#include "../copy_event_dialog.h"

#include <QTextStream>
#include <QApplication>
#include <QDialog>
#include <QInputDialog>
#include <QSettings>
#include <QTimer>

#include <cmath>
#include <algorithm>

namespace BioSig_
{

//-----------------------------------------------------------------------------
// TODO! constructor
SignalBrowserModel::SignalBrowserModel(FileSignalReader& reader,
                                       MainWindowModel& main_window_model,
                                       QSharedPointer<EventTableFileReader const> event_table_file_reader,
                                       ApplicationContext& app_context,
                                       FileContext& file_context,
                                       TabContext& tab_context)
: app_context_ (app_context),
  file_context_ (file_context),
  tab_context_ (tab_context),
  signal_browser_view_ (0),
  log_stream_(0),
  main_window_model_(main_window_model),
  state_(STATE_READY),
  mode_(MODE_POINTER),
  signal_buffer_ (reader),
  basic_header_(reader.getBasicHeader()),
  selected_event_item_ (0),
  event_table_file_reader_ (event_table_file_reader),
  release_buffer_(false),
  pixel_per_sec_(100),
  signal_height_(75),
  signal_spacing_(1),
  prefered_x_grid_pixel_intervall_(100),
  prefered_y_grid_pixel_intervall_(25),
  x_grid_pixel_intervall_(0),
  shown_event_types_ (event_table_file_reader->getAllEventTypes()),
  actual_event_creation_type_ (1),
  show_y_grid_(true)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
// destructor
SignalBrowserModel::~SignalBrowserModel()
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
// get main window modell
MainWindowModel& SignalBrowserModel::getMainWindowModel()
{
    return main_window_model_;
}

//-----------------------------------------------------------------------------
// set signal browser view
void SignalBrowserModel::setSignalBrowserView(SignalBrowserView* signal_browser_view)
{
    signal_browser_view_ = signal_browser_view;
}

//-----------------------------------------------------------------------------
// TODO! set log stream
void SignalBrowserModel::setLogStream(QTextStream* log_stream)
{
    log_stream_ = log_stream;
    // signal_buffer_.setLogStream(log_stream);
}

//-----------------------------------------------------------------------------
// get log stream
QTextStream& SignalBrowserModel::getLogStream()
{
    return *log_stream_;
}

//-----------------------------------------------------------------------------
// load settings
void SignalBrowserModel::loadSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("SignalBrowserModel");

    release_buffer_ = settings.value("release_buffer", release_buffer_).toBool();
    pixel_per_sec_ = settings.value("pixel_per_sec", pixel_per_sec_).toDouble();
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
    emit shownEventTypesChanged (shown_event_types_);
}

//-----------------------------------------------------------------------------
// save settings
void SignalBrowserModel::saveSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("SignalBrowserModel");

    settings.setValue("release_buffer", release_buffer_);
    settings.setValue("pixel_per_sec", pixel_per_sec_);
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
// TODO! set mode
void SignalBrowserModel::setMode(SignalBrowserMode mode)
{
    if (!checkSignalBrowserPtr("setMode") ||
        !checkReadyState("setMode"))
    {
        return;
    }

    mode_ = mode;
    //signal_browser_->getCanvasView()
    //                    ->setToolTipsEnabled(mode == MODE_POINTER ||
    //                                         mode == MODE_NEW);
}

//-----------------------------------------------------------------------------
// get mode
SignalBrowserMode SignalBrowserModel::getMode()
{
    return mode_;
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::setWholeDataBuffer(SignalBuffer::WHOLE_BUFFER
                                                whole_buffer)
{
    signal_buffer_.setWholeDataBuffer(whole_buffer);
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::enableInitDownsampling(bool enabled)
{
    signal_buffer_.enableInitDownsampling(enabled);
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::enableInitMinMaxSearch(bool enabled)
{
    signal_buffer_.enableInitMinMaxSearch(enabled);
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::setDefaultRange(float32 min, float32 max)
{
    signal_buffer_.setDefaultRange(min, max);
}



/*
// set release buffer
void SignalBrowserModel::setReleaseBuffer(bool release)
{
    release_buffer_ = release;
}

// get release buffer
bool SignalBrowserModel::getReleaseBuffer()
{
    return release_buffer_;
}
*/
// add signal view
void SignalBrowserModel::addChannel(uint32 channel_nr)
{
    if (!checkSignalBrowserPtr("addChannel") ||
        !checkReadyState("addChannel"))
    {
        return;
    }

    if (channel_nr >= basic_header_->getNumberChannels())
    {
        return; // illegal channel number
    }

    if(channel2signal_item_.find(channel_nr) != channel2signal_item_.end())
    {
         return; // already added
    }

    // generate signal item
    SignalGraphicsItem* signal_item
        = new SignalGraphicsItem(file_context_.getEventManager(),
                                 tab_context_,
                                 signal_buffer_, basic_header_->getChannel(channel_nr), *this,
                                 signal_browser_view_);

    channel2signal_item_[channel_nr] = signal_item;

    // add channel to buffer
    signal_buffer_.addChannel(channel_nr);
}

// remove signal view
void SignalBrowserModel::removeChannel(uint32 channel_nr)
{
    if (!checkSignalBrowserPtr("removeChannel") ||
        !checkReadyState("removeChannel"))
    {
        return;
    }

    Int2SignalGraphicsItemPtrMap::iterator sig_iter;

    sig_iter = channel2signal_item_.find(channel_nr);
    if(sig_iter == channel2signal_item_.end())
    {
         return; // not added
    }

    // remove signal canvas item
    signal_browser_view_->removeSignalGraphicsItem (channel_nr, sig_iter->second);
    channel2signal_item_.erase(sig_iter);
    delete sig_iter->second;

    // remove channel from buffer
    if (release_buffer_)
    {
        signal_buffer_.removeChannel(channel_nr);
    }
    else
    {
        signal_buffer_.setChannelActive(channel_nr, false);
    }
}

//-----------------------------------------------------------------------------
// is signal shown
bool SignalBrowserModel::isChannelShown(uint32 channel_nr) const
{
    return channel2signal_item_.find(channel_nr) != channel2signal_item_.end();
}

//-----------------------------------------------------------------------------
std::map<uint32, QString> SignalBrowserModel::getShownChannels () const
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
SignalBuffer const& SignalBrowserModel::getSignalBuffer () const
{
    return signal_buffer_;
}


//-----------------------------------------------------------------------------
// init buffer
void SignalBrowserModel::initBuffer()
{
    if (!checkSignalBrowserPtr("initBuffer") ||
        !checkReadyState("initBuffer"))
    {
        return;
    }

    // set all channel active
    Int2SignalGraphicsItemPtrMap::iterator iter;

    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        signal_buffer_.setChannelActive(iter->first, true);
    }

    // init buffer
    state_ = STATE_INIT_BUFFER;
    signal_browser_view_->hide();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    signal_buffer_.init();
    QApplication::restoreOverrideCursor();
    state_ = STATE_READY;


    // get range from buffer
    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        iter->second->getRangeFromBuffer(2.0);
    }


    // gernerate event items
    QList<EventID> event_ids = file_context_.getEventManager().getAllEvents();
    if (id2event_item_.size() == 0)
    {
        for (QList<EventID>::const_iterator event_id_iter = event_ids.begin ();
             event_id_iter != event_ids.end ();
             ++event_id_iter)
        {
            id2event_item_[*event_id_iter] = new EventGraphicsItem (
                    *this,
                    file_context_.getEventManager().getEvent (*event_id_iter),
                    app_context_,
                    file_context_.getEventManager(),
                    tab_context_);
        }
    }
}


//-----------------------------------------------------------------------------
// get viewing position
QPointF SignalBrowserModel::getViewingPosition ()
{
    return QPointF(static_cast<float>(signal_browser_view_->getVisibleX()) / pixel_per_sec_, signal_browser_view_->getVisibleY());
}

//-----------------------------------------------------------------------------
// set viewing position
void SignalBrowserModel::setViewingPosition (QPointF topleft)
{
    signal_browser_view_->goTo(topleft.x() * pixel_per_sec_, topleft.y());
}



//-----------------------------------------------------------------------------
// zoom in all
void SignalBrowserModel::zoomInAll()
{
    if (!checkSignalBrowserPtr("zoomInAll") ||
        !checkReadyState("zoomInAll"))
    {
        return;
    }

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
    if (!checkSignalBrowserPtr("zoomOutAll") ||
        !checkReadyState("zoomOutAll"))
    {
        return;
    }

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
// auto scale all
void SignalBrowserModel::autoScaleAll()
{
    if (!checkSignalBrowserPtr("autoScaleAll") ||
        !checkReadyState("autoScaleAll"))
    {
        return;
    }

    Int2SignalGraphicsItemPtrMap::iterator iter;

    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        iter->second->autoScale(auto_zoom_type_);
    }

    signal_browser_view_->updateWidgets();
}

// set pixel per sec
void SignalBrowserModel::setPixelPerXUnit(float64 pixel_per_sec)
{
    if (!checkSignalBrowserPtr("setPixelPerSec") ||
        !checkReadyState("setPixelPerSec"))
    {
        return;
    }
    pixel_per_sec_ = pixel_per_sec;
    signal_browser_view_->setPixelPerSec (pixel_per_sec_);
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
    int32 width = (int32)(signal_buffer_.getBlockDuration() *
                          signal_buffer_.getNumberBlocks() * pixel_per_sec_);

    int32 height = (signal_height_  + signal_spacing_) *
                   channel2signal_item_.size();

    QDialog* dialog = new QDialog;
    dialog->setWindowTitle("Initialize Chunk Matrix");
    dialog->resize(250, 50);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dialog->show();
    signal_browser_view_->resizeScene(width, height);
    QApplication::restoreOverrideCursor();
    dialog->hide();
    delete dialog;

    // singanls position
    channel2y_pos_.clear();
    channel2y_pos_[SignalEvent::UNDEFINED_CHANNEL] = 0;
    int32 y_pos = 0;
    Int2SignalGraphicsItemPtrMap::iterator signal_iter;

    for (signal_iter = channel2signal_item_.begin();
         signal_iter != channel2signal_item_.end();
         signal_iter++, y_pos += signal_height_ + signal_spacing_)
    {
        channel2y_pos_[signal_iter->first] = y_pos;
        signal_iter->second->setHeight (signal_height_);
        signal_iter->second->setPos (0, y_pos); // FIXME: why "/2" ????
        signal_iter->second->setZValue(SIGNAL_Z);
        signal_iter->second->updateYGridIntervall();

        signal_iter->second->enableYGrid(show_y_grid_);
        signal_iter->second->enableXGrid(show_x_grid_);
        signal_browser_view_->addSignalGraphicsItem(signal_iter->first, signal_iter->second);
        signal_iter->second->show();
    }

    updateEventItemsImpl ();

    // update x grid intervall
    float64 x_grid_intervall = round125(prefered_x_grid_pixel_intervall_ /
                                        pixel_per_sec_);

    x_grid_pixel_intervall_ = pixel_per_sec_ * x_grid_intervall;
    signal_browser_view_->setXAxisIntervall (x_grid_pixel_intervall_);
    signal_browser_view_->update();
}

//-------------------------------------------------------------------
void SignalBrowserModel::updateEventItems ()
{
    updateEventItemsImpl ();
    signal_browser_view_->update ();
}

//-------------------------------------------------------------------
void SignalBrowserModel::setActualEventCreationType (uint16 new_type)
{
    actual_event_creation_type_ = new_type;
}

//-------------------------------------------------------------------
void SignalBrowserModel::selectEvent (int32 id)
{
    Int2EventGraphicsItemPtrMap::iterator event_iter = id2event_item_.find (id);
    if (event_iter == id2event_item_.end())
    {
        tab_context_.setSelectionState(TAB_STATE_NO_EVENT_SELECTED);
        selected_event_item_ = 0;
        emit eventSelected (QSharedPointer<SignalEvent const>(0));
        return;
    }

    EventGraphicsItem* item = event_iter->second;

    if (file_context_.getEventManager().getEvent(id)->getChannel() ==
        SignalEvent::UNDEFINED_CHANNEL)
        tab_context_.setSelectionState(TAB_STATE_EVENT_SELECTED_ALL_CHANNELS);
    else
        tab_context_.setSelectionState (TAB_STATE_EVENT_SELECTED_ONE_CHANNEL);

    if (selected_event_item_)
        selected_event_item_->setSelected (false);
    selected_event_item_ = item;
    selected_event_item_->setSelected (true);

    emit eventSelected(item->getSignalEvent());
}

//-------------------------------------------------------------------
void SignalBrowserModel::unselectEvent ()
{    
    tab_context_.setSelectionState (TAB_STATE_NO_EVENT_SELECTED);
    if (selected_event_item_)
        selected_event_item_->setSelected (false);
    selected_event_item_ = 0;
    emit eventSelected (QSharedPointer<SignalEvent const>(0));
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

// check ready state
inline bool SignalBrowserModel::checkReadyState(const QString& function)
{
    if (state_ != STATE_READY)
    {
        *log_stream_ << "SignalBrowserModel::" << function << " "
                     << "Error: illegal state\n";
        return false;
    }

    return true;
}

// check signal browser ptr
bool SignalBrowserModel::checkSignalBrowserPtr(const QString function)
{
    if (!signal_browser_view_)
    {
        *log_stream_ << "SignalBrowserModel::" << function << " "
                     << "Error: SignalBrowser not set\n";
        return false;
    }

    return true;
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
                file_context_.getEventManager().getEvent(event_iter->first);
        if (!event)
        {
            *log_stream_ << "SignalBrowserModel::updateLayout Error: "
                         << "inconsistant events\n";
            continue;
        }

        Int2IntMap::iterator y_pos_iter;
        y_pos_iter = channel2y_pos_.find(event->getChannel());


        if (!shown_event_types_.count(event->getType()) ||
            y_pos_iter == channel2y_pos_.end())
        {
            event_iter->second->hide();
            if (event_iter->second == selected_event_item_)
            {
                selected_event_item_->setSelected(false);
                selected_event_item_ = 0;
                tab_context_.setSelectionState (TAB_STATE_NO_EVENT_SELECTED);
            }

            continue; // event type or channel not shown
        }
        event_iter->second->updateToSignalEvent ();

        signal_browser_view_->addEventGraphicsItem(event_iter->second);

        event_iter->second->show();
    }
}


//-----------------------------------------------------------------------------
// goTo
void SignalBrowserModel::goTo(float32 sec, int32 channel_index)
{
    float32 x = sec * pixel_per_sec_;
    float32 y = channel_index < 0 ? signal_browser_view_->getVisibleY()
                                : channel_index *
                                  (signal_height_ + signal_spacing_);
    signal_browser_view_->goTo(x, y);
    //signal_browser_view_->smoothGoTo (x, y);
}

//-----------------------------------------------------------------------------
// goToAndSelectNextEvent
void SignalBrowserModel::goToAndSelectNextEvent (bool forward)
{
    if (selected_event_item_)
    {
        QMap<int32, QSharedPointer<SignalEvent> > events (signal_buffer_.getEvents(selected_event_item_->getSignalEvent()->getType()));
        QMap<int32, QSharedPointer<SignalEvent> >::iterator current_event_iter = events.find(selected_event_item_->getSignalEvent()->getPosition());
        if (forward)
            ++current_event_iter;
        else if (current_event_iter != events.begin())
            --current_event_iter;
        else
            return;

        if (current_event_iter != events.end())
        {
            float x = (float)current_event_iter.value()->getPosition() / signal_buffer_.getEventSamplerate();
            //Int2EventGraphicsItemPtrMap::iterator event_graphics_item_it = id2event_item_.find(current_event_iter.value()->getId());
            //event_graphics_item_it.value()->setSelected (true);
            selectEvent (current_event_iter.value()->getId());

            int32 y = 0;
            goTo (x, y);
        }
    }
}

/*
// zoom rect
void SignalBrowserModel::zoomRect(const QRect& rect)
{
    SmartCanvasView* canvas_view = signal_browser_->getCanvasView();
    int32 visible_width = canvas_view->visibleWidth();
    int32 visible_height = canvas_view->visibleHeight();
    float64 sec = rect.x() / pixel_per_sec_;
    int32 y_signals = (int32)(rect.y() / (float64)(signal_height_ +
                                          signal_spacing_));

    pixel_per_sec_ *= visible_width / (float64)rect.width();
    pixel_per_sec_ = pixel_per_sec_ > visible_width ? visible_width
                                                   : pixel_per_sec_;

    float64 signals_per_page = visible_height / (signal_height_ +
                                                 signal_spacing_);

    signals_per_page *= rect.height() / (float64)visible_height;
    signal_height_ = (int32)(visible_height / ceil(signals_per_page));
    signal_height_ = signal_height_ > visible_height ? visible_height
                                                     : signal_height_;
    signal_height_ -= signal_spacing_;
    updateLayout();

    int32 x = (int32)(sec * pixel_per_sec_);
    int32 y = (int32)(y_signals * (signal_height_ + signal_spacing_));

    signal_browser_->getCanvasView()->setContentsPos(x, y);
    main_window_model_.pixelPerSecChanged(pixel_per_sec_);
    main_window_model_.signalHeightChanged(signal_height_);
}
*/

//-----------------------------------------------------------------------------
QString SignalBrowserModel::getEventName (uint16 event_type_id) const
{
    if (event_table_file_reader_.isNull())
        return QString ("");
    return event_table_file_reader_->getEventName (event_type_id);
}

//-----------------------------------------------------------------------------
QColor SignalBrowserModel::getEventColor (uint16 event_type_id) const
{
    return main_window_model_.getEventColorManager().getEventColor (event_type_id);
}


//-----------------------------------------------------------------------------
// get shown event types
void SignalBrowserModel::getShownEventTypes(IntList& event_type)
{
    foreach(uint16 shown_type, shown_event_types_)
        event_type.append(shown_type);
}

//-----------------------------------------------------------------------------
// get shown event types
std::set<uint16> SignalBrowserModel::getShownEventTypes () const
{
    return shown_event_types_;
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
void SignalBrowserModel::setShownEventTypes(const IntList& event_type, const bool all)
{
    shown_event_types_.clear();
    foreach(uint16 shown_type, event_type)
        shown_event_types_.insert(shown_type);
    all_event_types_selected_ = all;
    emit shownEventTypesChanged(shown_event_types_);
}


//-----------------------------------------------------------------------------
void SignalBrowserModel::addEventItem (QSharedPointer<SignalEvent const> event)
{
    EventGraphicsItem* event_item = new EventGraphicsItem(
            *this, event, app_context_, file_context_.getEventManager(),
            tab_context_);

    id2event_item_[event->getId()] = event_item;
    signal_browser_view_->addEventGraphicsItem(event_item);
    event_item->updateToSignalEvent();
    event_item->show();
    file_context_.setState (FILE_STATE_CHANGED);
}

//-----------------------------------------------------------------------------
void SignalBrowserModel::removeEventItem (EventID id)
{
    if (id2event_item_.find (id) == id2event_item_.end())
        return;

    file_context_.setState (FILE_STATE_CHANGED);
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

    file_context_.setState (FILE_STATE_CHANGED);
    EventGraphicsItem* event_item = id2event_item_[id];
    event_item->updateToSignalEvent ();
    if (selected_event_item_)
        if (selected_event_item_->getId() == id)
        {
            if (selected_event_item_->getSignalEvent()->getChannel() ==
                SignalEvent::UNDEFINED_CHANNEL)
                tab_context_.setSelectionState (TAB_STATE_EVENT_SELECTED_ALL_CHANNELS);
            else
                tab_context_.setSelectionState (TAB_STATE_EVENT_SELECTED_ONE_CHANNEL);

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

//-----------------------------------------------------------------------------
// set selected channel to all channels
void SignalBrowserModel::setSelectedEventToAllChannels()
{
    if (!selected_event_item_)
    {
        return;
    }

    EventID id = selected_event_item_->getId();
    QUndoCommand* changeChannelCommand = new ChangeChannelUndoCommand (file_context_.getEventManager(),
                                                                       id, SignalEvent::UNDEFINED_CHANNEL);
    tab_context_.executeCommand (changeChannelCommand);
}

//-----------------------------------------------------------------------------
// change selected event channel
void SignalBrowserModel::changeSelectedEventChannel()
{

    if (!selected_event_item_)
    {
        return;
    }

    EventID id = selected_event_item_->getId ();
    QSharedPointer<SignalEvent const> event =
            file_context_.getEventManager ().getEvent (id);

    // generate channel list
    int32 current_item = 0;
    QStringList channel_list;

    channel_list.append(tr("All Channels"));

    for (uint32 channel_nr = 0;
         channel_nr < basic_header_->getNumberChannels();
         channel_nr++)
    {
        if (isChannelShown(channel_nr))
        {
            if (event->getChannel() == (int32)channel_nr)
            {
                current_item = channel_list.size();
            }

            channel_list.append(
                QString("(%1) ").arg(channel_nr + 1) +
                basic_header_->getChannel(channel_nr).getLabel());
        }
    }

    // dialog
    bool ok = false;

    QString res = QInputDialog::getItem(signal_browser_view_, tr("Change Channel"),
                                        tr("Select new Channel:"),
                                        channel_list, current_item,
                                        false, &ok);

    int32 new_channel = res.replace(')', '(').section('(', 1, 1).toInt() - 1;

    if (ok && new_channel != event->getChannel())
    {
        QUndoCommand* changeChannelCommand = new ChangeChannelUndoCommand (file_context_.getEventManager(),
                                                                           id, new_channel);
        tab_context_.executeCommand (changeChannelCommand);
    }
}

//-----------------------------------------------------------------------------
// copy selected event to channels
void SignalBrowserModel::copySelectedEventToChannels()
{
    if (!selected_event_item_)
    {
        return;
    }

    uint32 id = selected_event_item_->getId();
    QSharedPointer<SignalEvent> event = signal_buffer_.getEvent(id);
    CopyEventDialog copy_event_dialog(basic_header_, signal_browser_view_);

    for (uint32 channel_nr = 0;
         channel_nr < basic_header_->getNumberChannels();
         channel_nr++)
    {
        if ((int32)channel_nr != event->getChannel() &&
            isChannelShown(channel_nr))
        {
            copy_event_dialog.addSelectableChannel(channel_nr);
        }
    }

    // show dialog
    copy_event_dialog.loadSettings();
    copy_event_dialog.exec();
    copy_event_dialog.saveSettings();

    if (copy_event_dialog.result() == QDialog::Rejected)
    {
        return; // user cancel
    }

    // generate copies
    for (uint32 channel_nr = 0;
         channel_nr < basic_header_->getNumberChannels();
         channel_nr++)
    {
        if (copy_event_dialog.isSelected(channel_nr))
        {
            QSharedPointer<SignalEvent> new_event = QSharedPointer<SignalEvent>(new SignalEvent(*event));
            new_event->setChannel(channel_nr);
            NewEventUndoCommand* new_event_command = new NewEventUndoCommand(file_context_.getEventManager(), new_event);
            tab_context_.executeCommand (new_event_command);
        }
    }
}

//-----------------------------------------------------------------------------
// change selected event type
void SignalBrowserModel::changeSelectedEventType (uint16 new_type)
{
    QSharedPointer<SignalEvent const> event (0);

    if (selected_event_item_)
        event = selected_event_item_->getSignalEvent();

    if (!event.isNull())
    {
        if (new_type != event->getType())
        {
            ChangeTypeUndoCommand* change_type_command = new ChangeTypeUndoCommand (file_context_.getEventManager(), event->getId(), new_type);
            tab_context_.executeCommand (change_type_command);
        }
    }
}

//-----------------------------------------------------------------------------
uint16 SignalBrowserModel::getActualEventCreationType () const
{
    return actual_event_creation_type_;
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


// TODO QT4: IMPLEMENT!!!!
bool SignalBrowserModel::isShowAllEventTypes() const
{
 /*   return all_event_types_selected_;*/
    return false; /// TODO QT4 : RETURN all_event_types_selected_
}

} // namespace BioSig_
