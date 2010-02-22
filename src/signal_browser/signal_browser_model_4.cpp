// signal_browser_model.cpp

#include "signal_browser_model_4.h"
#include "signal_browser_view.h"
#include "signal_graphics_item.h"
#include "x_axis_widget_4.h"
#include "event_graphics_item.h"
#include "change_channel_undo_command.h"
#include "change_type_undo_command.h"
#include "new_event_undo_command.h"

#include "../command_stack.h"
#include "../main_window_model.h"
#include "../event_color_manager.h"
#include "../label_widget.h"
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
                                       QSharedPointer<EventTableFileReader const> event_table_file_reader)
: signal_browser_view_ (0),
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

    all_event_types_selected_ = settings.value("all_event_types_selected", all_event_types_selected_).toBool();
    int size = settings.beginReadArray("shown_event_types");
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);
        bool ok = false;
        uint16 event_type = settings.value("shown_event_type").toInt(&ok);
        if (ok)
            shown_event_types_.insert(event_type);
    }
    settings.endArray();

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

    settings.setValue("all_event_types_selected", all_event_types_selected_);
    settings.beginWriteArray("shown_event_types", shown_event_types_.size());
    unsigned array_index = 0;
    foreach (uint16 event_type, shown_event_types_)
    {
        settings.setArrayIndex(array_index);
        settings.setValue("shown_event_type", event_type);
        array_index++;
    }
    settings.endArray();

    settings.endGroup();
}

//-----------------------------------------------------------------------------
// TODO! set mode
void SignalBrowserModel::setMode(Mode mode)
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
SignalBrowserModel::Mode SignalBrowserModel::getMode()
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
        = new SignalGraphicsItem(signal_buffer_, basic_header_->getChannel(channel_nr), *this,
                               signal_browser_view_);

    channel2signal_item_[channel_nr] = signal_item;
    connect (signal_item, SIGNAL(mouseAtSecond(float64)), &(signal_browser_view_->getXAxisWidget()), SLOT(changeHighlightTime(float64)));
    connect (signal_item, SIGNAL(mouseMoving(bool)), &(signal_browser_view_->getXAxisWidget()), SLOT(enableHighlightTime(bool)));

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
bool SignalBrowserModel::isChannelShown(uint32 channel_nr)
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
uint32 SignalBrowserModel::getNumberShownChannels()
{
    return channel2signal_item_.size();
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
    uint32 number_events = signal_buffer_.getNumberEvents();

    if (id2event_item_.size() == 0)
    {
        for (uint32 event_nr = 0; event_nr < number_events; event_nr++)
        {
            int32 event_id = signal_buffer_.eventNumber2ID(event_nr);
            id2event_item_[event_id] = QSharedPointer<EventGraphicsItem> (new EventGraphicsItem (signal_buffer_,
                                                           *this,
                                                           signal_buffer_.getEvent(event_nr)));
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
void SignalBrowserModel::setPixelPerSec(float64 pixel_per_sec)
{
    if (!checkSignalBrowserPtr("setPixelPerSec") ||
        !checkReadyState("setPixelPerSec"))
    {
        return;
    }
    pixel_per_sec_ = pixel_per_sec;
    signal_browser_view_->getXAxisWidget().changePixelPerSec (pixel_per_sec_);
    updateLayout();
}

// get pixel per sec
float64 SignalBrowserModel::getPixelPerSec() const
{
    return pixel_per_sec_;
}



// set signal height
void SignalBrowserModel::setSignalHeight(int32 height)
{
    signal_height_ = height;
    emit signalHeightChanged (height);
    updateLayout();
    // TODO: reimplement!
//    if (!checkSignalBrowserPtr("setSignalHeight") ||
//        !checkReadyState("setSignalHeight"))
//    {
//        return;
//    }
//
//    int32 x = signal_browser_->getCanvasView()->contentsX();
//    int32 y  = (int32)(signal_browser_->getCanvasView()->contentsY() *
//                       (height + signal_spacing_) /
//                       (signal_height_ + signal_spacing_));
//
//    signal_height_ = height;
//    updateLayout();
//    signal_browser_->getCanvasView()->setContentsPos(x, y);
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

    // TODO: implement other items!!!
    /*
    // navigation item
    navigation_item_->setSize(width, height);
    navigation_item_->setX(0);
    navigation_item_->setY(0);
    navigation_item_->setZ(NAVIGATION_Z);
    navigation_item_->show();

    // x grid
    x_grid_item_->setSize(width, height);
    x_grid_item_->setX(0);
    x_grid_item_->setY(0);
    x_grid_item_->setZ(X_GRID_Z);

    x_grid_item_->setVisible(show_x_grid_);

    // channel separator
    channel_separator_item_->setSize(width, height);
    channel_separator_item_->setX(0);
    channel_separator_item_->setY(0);
    channel_separator_item_->setZ(CHANNEL_SEPARATOR_Z);
    channel_separator_item_->show();
    */
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
//        signal_iter->second->setCacheMode (QGraphicsItem::NoCache);
        signal_iter->second->setHeight (signal_height_);
        signal_iter->second->setPos (0, y_pos); // FIXME: why "/2" ????
        signal_iter->second->setZValue(SIGNAL_Z);
        signal_iter->second->updateYGridIntervall();

        signal_iter->second->enableYGrid(show_y_grid_);
        signal_iter->second->enableXGrid(show_x_grid_);
        signal_browser_view_->addSignalGraphicsItem(signal_iter->first, signal_iter->second);
        signal_iter->second->show();

//        signal_iter->second->setCacheMode (QGraphicsItem::DeviceCoordinateCache);
    }

    updateEventItemsImpl ();

    // update x grid intervall
    float64 x_grid_intervall = round125(prefered_x_grid_pixel_intervall_ /
                                        pixel_per_sec_);

    x_grid_pixel_intervall_ = pixel_per_sec_ * x_grid_intervall;
    signal_browser_view_->getXAxisWidget().changeTotalLengthInSecs(signal_buffer_.getBlockDuration() *
                                                                   signal_buffer_.getNumberBlocks());
    signal_browser_view_->getXAxisWidget().changeIntervall (x_grid_pixel_intervall_);
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
    if (!id2event_item_.contains (id))
    {
        main_window_model_.setSelectionState(
                                MainWindowModel::SELECTION_STATE_NONE);
        selected_event_item_ = QSharedPointer<EventGraphicsItem>(0);
        emit eventSelected (QSharedPointer<SignalEvent>(0));
        return;
    }

    QSharedPointer<EventGraphicsItem> item = id2event_item_.find (id).value();
    if (item.isNull())
    {
        main_window_model_.setSelectionState(
                                MainWindowModel::SELECTION_STATE_NONE);
        selected_event_item_ = QSharedPointer<EventGraphicsItem>(0);
        emit eventSelected (QSharedPointer<SignalEvent>(0));
        return;
    }

    if (signal_buffer_.getEvent(item->getId())->getChannel() ==
        SignalEvent::UNDEFINED_CHANNEL)
    {
        main_window_model_.setSelectionState(
                                MainWindowModel::SELECTION_STATE_ALL_CHANNELS);
    }
    else
    {
        main_window_model_.setSelectionState(
                                MainWindowModel::SELECTION_STATE_ONE_CHANNEL);
    }

    if (!selected_event_item_.isNull())
        selected_event_item_->setSelected (false);
    selected_event_item_ = item;
    selected_event_item_->setSelected (true);

    emit eventSelected(item->getSignalEvent());
}

//-------------------------------------------------------------------
void SignalBrowserModel::unselectEvent ()
{    
    if (!selected_event_item_.isNull())
        selected_event_item_->setSelected (false);
    selected_event_item_ = QSharedPointer<EventGraphicsItem>(0);
    emit eventSelected (QSharedPointer<SignalEvent>(0));
}


/*
// set signal spacing
void SignalBrowserModel::setSignalSpacing(int32 spacing)
{
    if (!checkSignalBrowserPtr("setSignalSpacing") ||
        !checkReadyState("setSignalSpacing"))
    {
        return;
    }

    signal_spacing_ = spacing;
    updateLayout();
}
*/
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
/*
// canvas view content moving
void SignalBrowserModel::canvasViewContentMoving(int content_x, int content_y)
{
    if (!checkSignalBrowserPtr("canvasViewContentMoving") ||
        !checkReadyState("canvasViewContentMoving"))
    {
        return;
    }

    SmartCanvasView* canvas_view = signal_browser_->getCanvasView();
    int32 visible_height = canvas_view->visibleHeight();
    int32 signal_view_y = 0;
    Int2SignalCanvasItemPtrMap::iterator iter;

    for (iter = channel2signal_item_.begin();
         iter != channel2signal_item_.end();
         iter++)
    {
        if (signal_view_y + signal_height_ > content_y &&
            signal_view_y < content_y + visible_height)
        {
            signal_buffer_.setChannelActive(iter.key(), true);
        }
        else
        {
            signal_buffer_.setChannelActive(iter.key(), false); // prediction
        }

        signal_view_y += signal_height_ + signal_spacing_;
    }
}

// set prefered x grid pixel intervall
void SignalBrowserModel::setPreferedXGirdPixelIntervall(int32 pixel_intervall)
{
    if (!checkSignalBrowserPtr("setPreferedXGirdPixelIntervall") ||
        !checkReadyState("setPreferedXGirdPixelIntervall"))
    {
        return;
    }

    prefered_x_grid_pixel_intervall_ = pixel_intervall;
    updateLayout();
}

// set prefered y grid pixel intervall
void SignalBrowserModel::setPreferedYGirdPixelIntervall(int32 pixel_intervall)
{
    if (!checkSignalBrowserPtr("setPreferedYGirdPixelIntervall") ||
        !checkReadyState("setPreferedYGirdPixelIntervall"))
    {
        return;
    }

    prefered_y_grid_pixel_intervall_ = pixel_intervall;
    updateLayout();
}

// get prefered x grid pixel intervall
int32 SignalBrowserModel::getPreferedXGirdPixelIntervall()
{
    return prefered_x_grid_pixel_intervall_;
}
*/
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
    int32 height = (signal_height_  + signal_spacing_) *
                   channel2signal_item_.size();

    Int2EventGraphicsItemPtrMap::iterator event_iter;
    for (event_iter = id2event_item_.begin();
         event_iter != id2event_item_.end();
         event_iter++)
    {
        // std::cout << "blub" << std::endl;
        QSharedPointer<SignalEvent> event = signal_buffer_.getEvent(event_iter.key());
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
            event_iter.value()->hide();
            if (event_iter.value() == selected_event_item_)
            {
                selected_event_item_->setSelected(false);
                selected_event_item_.clear();
                main_window_model_.setSelectionState(
                                        MainWindowModel::SELECTION_STATE_NONE);
            }

            continue; // event type or channel not shown
        }

        int32 event_width = (int32)(pixel_per_sec_ *
                                    (float64)event->getDuration() /
                                    signal_buffer_.getEventSamplerate() + 0.5);

        event_width = std::max(event_width, 1);

        if (event->getChannel() == SignalEvent::UNDEFINED_CHANNEL)
            event_iter.value()->setSize(event_width, height);
        else
            event_iter.value()->setSize(event_width, signal_height_);

        event_iter.value()->setZValue(EVENT_Z + event->getType() / 100000.0);
        int32 event_x = (int32)(pixel_per_sec_ * (float64)event->getPosition() /
                                signal_buffer_.getEventSamplerate() + 0.5);

        event_iter.value()->setPos(event_x, y_pos_iter->second);
        event_iter.value()->updateColor();

        signal_browser_view_->addEventGraphicsItem(event_iter.value());

        event_iter.value()->show();
        connect (event_iter.value().data(), SIGNAL(mouseAtSecond(float64)), &(signal_browser_view_->getXAxisWidget()), SLOT(changeHighlightTime(float64)));
        connect (event_iter.value().data(), SIGNAL(mouseMoving(bool)), &(signal_browser_view_->getXAxisWidget()), SLOT(enableHighlightTime(bool)));
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
    if (!selected_event_item_.isNull())
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
        if (event_it.value()->isVisible())
            present_event_types.insert (event_it.value()->getSignalEvent()->getType());
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
// set event changed
void SignalBrowserModel::setEventChanged(int32 id)
{
    if (!checkSignalBrowserPtr("setEventChanged") ||
        !checkReadyState("setEventChanged"))
    {
        return;
    }

    QSharedPointer<SignalEvent> event = signal_buffer_.getEvent(id);
    if (!event)
    {
        *log_stream_ << "SignalBrowserModel::setEventChanged "
                     << "Error: invalid event-id\n";
        return;
    }

    QSharedPointer<EventGraphicsItem> event_item = id2event_item_[id];
    Int2IntMap::iterator y_pos_iter;

    y_pos_iter = channel2y_pos_.find(event->getChannel());

    if (!shown_event_types_.count(event->getType()) ||
        y_pos_iter == channel2y_pos_.end())
    {
        event_item->hide(); // event type or channel not shown
    }
    else
    {
        int32 height = (signal_height_  + signal_spacing_) *
                        channel2signal_item_.size();
        int32 event_width = (int32)(pixel_per_sec_ *
                                    (float64)event->getDuration() /
                                    signal_buffer_.getEventSamplerate() + 0.5);

        event_width = std::max(event_width, 1);
        event_item->setPos(event_item->pos().x(), y_pos_iter->second);

        if (event->getChannel() == SignalEvent::UNDEFINED_CHANNEL)
        {
            event_item->setSize(event_width, height);
        }
        else
        {
            event_item->setSize(event_width, signal_height_);
        }

        event_item->setZValue(EVENT_Z + event->getType() / 100000.0);
        int32 event_x = (int32)(pixel_per_sec_ * (float64)event->getPosition() /
                                signal_buffer_.getEventSamplerate() + 0.5);

        event_item->setPos(event_x, event_item->pos().y());
        event_item->updateColor();
        event_item->show();
        signal_browser_view_->addEventGraphicsItem(event_item);
    }

    signal_browser_view_->updateWidgets();
    main_window_model_.setChanged();
    if (!selected_event_item_.isNull())
        if (selected_event_item_->getId() == id)
            eventSelected(event);
}

//-----------------------------------------------------------------------------
// remove event
void SignalBrowserModel::removeEvent(uint32 id, bool update)
{
    Int2EventGraphicsItemPtrMap::iterator it = id2event_item_.find(id);

    if (it == id2event_item_.end())
    {
        return;
    }

    if (!selected_event_item_.isNull())
        unselectEvent();

    QSharedPointer<EventGraphicsItem> event_item = it.value();

    id2event_item_.erase(it);
    // delete event_item;
    signal_buffer_.removeEvent(id);
    signal_browser_view_->removeEventGraphicsItem(event_item);

    if (update)
    {
        signal_browser_view_->updateWidgets();
    }

    main_window_model_.setChanged();
    main_window_model_.setSelectionState(MainWindowModel::SELECTION_STATE_NONE);
}

//-----------------------------------------------------------------------------
// add event
QSharedPointer<EventGraphicsItem> SignalBrowserModel::addEvent(QSharedPointer<SignalEvent> event,
                                              bool update)
{
    int32 event_id = signal_buffer_.addEvent(event);
    QSharedPointer<EventGraphicsItem> event_item (new EventGraphicsItem(signal_buffer_,
                                                      *this, event));

    id2event_item_[event_id] = event_item;
    setEventChanged(event_id);

    resetEventSizeAndPos(event_item);
    signal_browser_view_->addEventGraphicsItem(event_item);
    connect (event_item.data(), SIGNAL(mouseAtSecond(float64)), &(signal_browser_view_->getXAxisWidget()), SLOT(changeHighlightTime(float64)));
    connect (event_item.data(), SIGNAL(mouseMoving(bool)), &(signal_browser_view_->getXAxisWidget()), SLOT(enableHighlightTime(bool)));

    event_item->show();
    return event_item;
}

//-----------------------------------------------------------------------------
// add event
void SignalBrowserModel::addEvent(QSharedPointer<EventGraphicsItem> event)
{
    int32 event_id = signal_buffer_.addEvent (event->getSignalEvent());
    //event->set
    id2event_item_[event_id] = event;
    resetEventSizeAndPos(event);
    signal_browser_view_->addEventGraphicsItem(event);
    event->show();
}

//-----------------------------------------------------------------------------
// unset selected event item
void SignalBrowserModel::unsetSelectedEventItem()
{
    selected_event_item_.clear();
    main_window_model_.setSelectionState(MainWindowModel::SELECTION_STATE_NONE);
}


//-----------------------------------------------------------------------------
// get selected event item
QSharedPointer<EventGraphicsItem> SignalBrowserModel::getSelectedEventItem()
{
    return selected_event_item_;
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent> SignalBrowserModel::getSelectedSignalEvent()
{
    return selected_event_item_->getSignalEvent();
}


//-----------------------------------------------------------------------------
// get event item
QSharedPointer<EventGraphicsItem> SignalBrowserModel::getEventItem(int32 id)
{
    Int2EventGraphicsItemPtrMap::iterator it = id2event_item_.find(id);
    if (it != id2event_item_.end())
        return *it;
    else
        return QSharedPointer<EventGraphicsItem>(0);
}

//-----------------------------------------------------------------------------
// set selected channel to all channels
void SignalBrowserModel::setSelectedEventToAllChannels()
{
    if (!selected_event_item_)
    {
        return;
    }

    uint32 id = selected_event_item_->getId();
    QSharedPointer<SignalEvent> event = signal_buffer_.getEvent(id);

    QUndoCommand* changeChannelCommand = new ChangeChannelUndoCommand (*this, event, SignalEvent::UNDEFINED_CHANNEL);
    CommandStack::instance().executeEditCommand(changeChannelCommand);
}

//-----------------------------------------------------------------------------
// change selected event channel
void SignalBrowserModel::changeSelectedEventChannel()
{

    if (!selected_event_item_)
    {
        return;
    }

    uint32 id = selected_event_item_->getId();
    QSharedPointer<SignalEvent> event = signal_buffer_.getEvent(id);

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
        QUndoCommand* changeChannelCommand = new ChangeChannelUndoCommand (*this, event, new_channel);
        CommandStack::instance().executeEditCommand(changeChannelCommand);
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
            NewEventUndoCommand* new_event_command = new NewEventUndoCommand(*this, new_event);
            CommandStack::instance().executeEditCommand (new_event_command);
        }
    }

    signal_browser_view_->updateWidgets();
}

//-----------------------------------------------------------------------------
// change selected event type
void SignalBrowserModel::changeSelectedEventType()
{
    QSharedPointer<SignalEvent> event (0);

    if (selected_event_item_)
        event = signal_buffer_.getEvent(selected_event_item_->getId());

    // generate list show all shown types
    QStringList event_type_list;
    int32 current_item = 0;

    for (std::set<uint16>::const_iterator it = shown_event_types_.begin();
         it != shown_event_types_.end();
         it++)
    {
        if (event)
        {
            if (event->getType() == *it)
                current_item = event_type_list.size();
        }
        else
            if (actual_event_creation_type_ == *it)
                current_item = event_type_list.size();

        QString event_name
             = main_window_model_.getEventTableFileReader()->getEventName(*it);

            event_type_list.append(event_name + " " + QString("(%1)")
                                                        .arg(*it,4, 16)
                                                        .replace(' ', '0'));
    }

    // dialog
    bool ok = false;

    QString res = QInputDialog::getItem(signal_browser_view_, tr("Change Type"),
                                        tr("Select new Type:"),
                                        event_type_list, current_item,
                                        false, &ok);

    uint16 new_type = res.right(5).left(4).toUShort(0, 16);

    if (event)
    {
        if (ok && new_type != event->getType())
        {
            ChangeTypeUndoCommand* change_type_command = new ChangeTypeUndoCommand (event, new_type);
            connect (change_type_command, SIGNAL(eventChanged(int32)), this, SLOT(setEventChanged(int32)));
            CommandStack::instance().executeEditCommand(change_type_command);
        }
    }

    else
        if (ok)
            actual_event_creation_type_ = new_type;
}

//-----------------------------------------------------------------------------
uint16 SignalBrowserModel::getActualEventCreationType () const
{
    return actual_event_creation_type_;
}


//-----------------------------------------------------------------------------
// remove selected event
void SignalBrowserModel::removeSelectedEvent()
{
    if (!selected_event_item_)
    {
        return;
    }

    uint32 id = selected_event_item_->getId();

    selected_event_item_.clear();
    main_window_model_.setSelectionState(MainWindowModel::SELECTION_STATE_NONE);
    removeEvent(id);
}

/*
// get channel nr
int32 SignalBrowserModel::getChannelNr(uint32 shown_nr)
{
    if (static_cast<int32>(shown_nr) >= channel2signal_item_.size())
    {
        return -1;
    }

    Int2SignalCanvasItemPtrMap::iterator it = channel2signal_item_.begin();

    while (shown_nr > 0)
    {
        it++;
        shown_nr--;
    }

    return it.key();
}
*/
// get events
void SignalBrowserModel::getEvents(SignalEventVector& event_vector)
{
    int32 number_events = signal_buffer_.getNumberEvents();

    for (int event_nr = 0; event_nr < number_events; event_nr++)
    {
        int32 event_id = signal_buffer_.eventNumber2ID(event_nr);
        event_vector.push_back(*signal_buffer_.getEvent(event_id));
    }
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

//-----------------------------------------------------------------------------
void SignalBrowserModel::resetEventSizeAndPos (QSharedPointer<EventGraphicsItem> event)
{
    QSharedPointer<SignalEvent> signal_event (event->getSignalEvent());
    Int2IntMap::iterator y_pos_iter = channel2y_pos_.find(signal_event->getChannel());
    int32 height = (signal_height_  + signal_spacing_) *
                   channel2signal_item_.size();


    if (!shown_event_types_.count(signal_event->getType()) ||
        y_pos_iter == channel2y_pos_.end())
    {
        event->hide();
        return;
    }

    int32 event_width = (int32)(pixel_per_sec_ *
                               (float64)signal_event->getDuration() /
                               signal_buffer_.getEventSamplerate() + 0.5);

    event_width = std::max(event_width, 1);

    if (signal_event->getChannel() == SignalEvent::UNDEFINED_CHANNEL)
        event->setSize(event_width, height);
    else
        event->setSize(event_width, signal_height_);

    event->setZValue(EVENT_Z + signal_event->getType() / 100000.0);
    int32 event_x = (int32)(pixel_per_sec_ * (float64)signal_event->getPosition() /
                            signal_buffer_.getEventSamplerate() + 0.5);

    event->setPos(event_x, y_pos_iter->second);
    event->updateColor();
    //event->show();
}


} // namespace PortingToQT4_
