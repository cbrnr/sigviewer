// main_window_model.cpp

#include "main_window_model_impl.h"
#include "main_window.h"
#include "tab_context.h"
#include "file_context.h"
#include "application_context.h"
#include "gui_action_manager.h"

#include "file_handling/file_signal_reader_factory.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "gui_impl/gui_helper_functions.h"
#include "gui_impl/open_file_gui_command.h"
#include "base/signal_event.h"
#include "basic_header_info_dialog.h"
#include "log_dialog.h"
#include "gui_impl/channel_selection_dialog.h"
#include "event_time_selection_dialog.h"
#include "gui_impl/dialogs/event_type_dialog.h"
#include "settings_dialog.h"
#include "command_stack.h"

#include "abstract_browser_model.h"

#include "signal_browser/signal_browser_model_4.h"
#include "signal_browser/signal_browser_view.h"
#include "signal_browser/calculate_event_mean_command.h"
#include "signal_browser/calculcate_frequency_spectrum_command.h"
#include "editing_commands/new_event_undo_command.h"
#include "block_visualisation/blocks_visualisation_view.h"
#include "block_visualisation/blocks_visualisation_model.h"

#include <QInputDialog>
#include <QString>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QAction>
#include <QTextStream>
#include <QSettings>
#include <QSharedPointer>
#include <QObject>
#include <QTime>
#include <QProgressDialog>

#include <cmath>
#include <iostream>

namespace BioSig_
{

int const MainWindowModelImpl::NUMBER_RECENT_FILES_ = 8;


// constructor
MainWindowModelImpl::MainWindowModelImpl ()
: main_window_(0),
  application_context_ (ApplicationContext::getInstance()),
  signal_browser_model_ (0),
  signal_browser_ (0),
  tab_widget_ (0),
  overflow_detection_ (false)
{
    log_stream_.reset(new QTextStream(&log_string_));
}

// destructor
MainWindowModelImpl::~MainWindowModelImpl()
{
    // nothing
}

// get log stream
QTextStream& MainWindowModelImpl::getLogStream()
{
    return *log_stream_.get();
}

// set main window
void MainWindowModelImpl::setMainWindow (MainWindow* main_window)
{
    main_window_ = main_window;
    application_context_->setState(APP_STATE_NO_FILE_OPEN);
}

// void load settings
void MainWindowModelImpl::loadSettings()
{
    if (checkMainWindowPtr("loadSettings"))
    {
        main_window_->loadSettings();
    }

    QSettings settings("SigViewer");
    settings.beginGroup("MainWindowModelImpl");

    int size = settings.beginReadArray("recent_file");
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);
        recent_file_list_.append(settings.value("name").toString());
    }

    settings.endArray();

    overflow_detection_ = settings.value("overflow_detection", overflow_detection_).toBool();

    settings.endGroup();
}

// void save settings
void MainWindowModelImpl::saveSettings()
{
    if (checkMainWindowPtr("saveSettings"))
    {
        main_window_->saveSettings();
    }

    QSettings settings("SigViewer");
    settings.beginGroup("MainWindowModelImpl");
    settings.beginWriteArray("recent_file");

    for (int i = 0; i < recent_file_list_.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("name", recent_file_list_.at(i));
    }

    settings.endArray();
    settings.setValue("overflow_detection", overflow_detection_);
    settings.endGroup();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::tabChanged (int tab_index)
{
    if (tab_contexts_.find(tab_index) != tab_contexts_.end ())
    {
        ApplicationContext::getInstance()->setCurrentTabContext (tab_contexts_[tab_index]);
        tab_contexts_[tab_index]->gotActive ();
    }
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::closeTab (int tab_index)
{
    if (tab_index == 0)
    {
        fileCloseAction();
        return;
    }
    QWidget* widget = tab_widget_->widget (tab_index);
    browser_models_.erase (tab_index);
    tab_widget_->removeTab (tab_index);
    delete widget;
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::calculateMeanAction ()
{
    if (signal_browser_model_.isNull())
        return;
    std::map<uint32, QString> shown_channels = signal_browser_model_->getShownChannelsWithLabels ();
    std::set<uint16> displayed_event_types = signal_browser_model_->getDisplayedEventTypes ();
    std::map<uint16, QString> shown_event_types;
    for (std::set<uint16>::iterator event_type_it = displayed_event_types.begin();
         event_type_it != displayed_event_types.end();
         ++event_type_it)
    {
        shown_event_types[*event_type_it] = ApplicationContext::getInstance()->getEventTableFileReader()->getEventName (*event_type_it);
    }

    EventTimeSelectionDialog event_time_dialog (shown_event_types, shown_channels,
                                                current_file_context_->getEventManager());
    if (event_time_dialog.exec () == QDialog::Rejected)
        return;
    else
    {
        CalculateEventMeanCommand command (current_file_context_->getEventManager(),
                                           current_file_context_->getChannelManager(),
                                           *this,
                                           event_time_dialog.getSelectedEventType(),
                                           event_time_dialog.getSelectedChannels(),
                                           event_time_dialog.getSecondsBeforeEvent(),
                                           event_time_dialog.getLengthInSeconds());
        command.execute();
    }
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::calculateFrequencySpectrumAction ()
{
    if (signal_browser_model_.isNull())
        return;
    std::map<uint32, QString> shown_channels = signal_browser_model_->getShownChannelsWithLabels ();
    std::set<uint16> displayed_event_types = signal_browser_model_->getDisplayedEventTypes ();
    std::map<uint16, QString> shown_event_types;
    for (std::set<uint16>::iterator event_type_it = displayed_event_types.begin();
         event_type_it != displayed_event_types.end();
         ++event_type_it)
    {
        shown_event_types[*event_type_it] = ApplicationContext::getInstance()->getEventTableFileReader()->getEventName (*event_type_it);
    }

    EventTimeSelectionDialog event_time_dialog (shown_event_types, shown_channels,
                                                current_file_context_->getEventManager());
    if (event_time_dialog.exec () == QDialog::Rejected)
        return;
    else
    {
        CalculcateFrequencySpectrumCommand command (signal_browser_model_, *this,
                                                    event_time_dialog.getSelectedEventType(),
                                                    event_time_dialog.getSelectedChannels(),
                                                    event_time_dialog.getSecondsBeforeEvent(),
                                                    event_time_dialog.getLengthInSeconds());
        command.execute();
    }
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::undoViewAction()
{
    CommandStack::instance().undoLastViewCommand();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::redoViewAction()
{
    CommandStack::instance().redoLastUndoneViewCommand();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::undoAction()
{
    tab_contexts_[tab_widget_->currentIndex()]->undo ();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::redoAction()
{
    tab_contexts_[tab_widget_->currentIndex()]->redo ();
}

// file import events action
void MainWindowModelImpl::fileImportEventsAction()
{
    if (!checkMainWindowPtr("fileImportEventsAction"))
    {
        return;
    }

    // import dialog
    QSettings settings("SigViewer");
    QString path = settings.value("MainWindowModelImpl/file_open_path", ".").toString();
    QString extensions = FileSignalReaderFactory::getInstance()->getExtensions();
    QString file_name;

    file_name = main_window_->showImportDialog(path, extensions);

    if (file_name.isEmpty())
    {
        return; // user cancel
    }

    // open signal reader
    FileSignalReader* signal_reader = 0;

    QString load;
    if (file_name.lastIndexOf('.') != -1)
    {
        signal_reader = FileSignalReaderFactory::getInstance()
                    ->getElement(file_name.mid(file_name.lastIndexOf('.')));

        if (signal_reader)
        {
            signal_reader->setLogStream(log_stream_.get());
            load = signal_reader->open(file_name, overflow_detection_);

            if (load.size())
            {
                delete signal_reader;
            }
        }
    }

    if (load.size() ||
        signal_reader->getBasicHeader()->getEventSamplerate() !=
        file_signal_reader_->getBasicHeader()->getEventSamplerate())
    {
        *log_stream_ << "MainWindowModelImpl::fileImportEventsAction Error: "
                     << "file format: '" << file_name
                     << "'\n";

        main_window_->showErrorReadDialog(file_name + "\"" + load + "\"");
        return;
    }

    FileSignalReader::SignalEventVector event_vector;
    signal_reader->loadEvents(event_vector);
    signal_reader->close();
    delete signal_reader;

    // get event types of import data
    EventTypeDialog::IntList event_types;
    FileSignalReader::SignalEventVector::iterator it;

    for (it = event_vector.begin(); it != event_vector.end(); it++)
    {
        event_types << it->getType();
    }

    qSort(event_types);

    // event type dialog
    EventTypeDialog event_type_dialog(tr("Import Events"),
                                      main_window_);

    event_type_dialog.setShownTypes(event_types);
    event_type_dialog.loadSettings();
    event_type_dialog.exec();
    event_type_dialog.saveSettings();

    if (event_type_dialog.result() == QDialog::Rejected)
    {
        return; // user cancel
    }

    event_type_dialog.getShownTypes(event_types);

    // import events
    bool inconsistent = false;

    float64 last_event_pos = file_signal_reader_->getBasicHeader()->getNumberRecords() *
                             file_signal_reader_->getBasicHeader()->getRecordDuration() *
                             file_signal_reader_->getBasicHeader()->getEventSamplerate();

    uint32 number_channels = file_signal_reader_->getBasicHeader()->getNumberChannels();

    for (it = event_vector.begin(); it != event_vector.end(); it++)
    {
        if (event_types.contains((*it).getType()))
        {
            event_manager_->createEvent (it->getChannel(), it->getPosition (),
                                         it->getDuration(), it->getType ());
            //signal_browser_model_->addEvent(QSharedPointer<SignalEvent>(new SignalEvent(*it)), false);
            if (!inconsistent && number_channels > 0 &&
                ((*it).getChannel() >= number_channels))
                // ||
                // (*it).getPosition() + (*it).getDuration() > last_event_pos))
            {
        fprintf(stdout,"inc=%i,ns=%i,getchan=%i,getPos+Dur=%i,lastPos=%f\n",inconsistent, number_channels,(*it).getChannel(),   (*it).getPosition() + (*it).getDuration(), last_event_pos);
                inconsistent = true;
            }
        }
    }

    if (inconsistent)
    {
        main_window_->showInconsistentEventsDialog();
    }

    signal_browser_model_->updateLayout();
}

// recent file menu about to show
void MainWindowModelImpl::recentFileMenuAboutToShow()
{
    main_window_->setRecentFiles(recent_file_list_);
}

// recent file activated
void MainWindowModelImpl::recentFileActivated(QAction* recent_file_action)
{
    if (!checkMainWindowPtr("recentFileActivated"))
    {
        return;
    }

    OpenFileGuiCommand::openFile (recent_file_action->text());
}


// file close action
void MainWindowModelImpl::fileCloseAction()
{
    if (!checkMainWindowPtr("fileCloseAction"))
    {
        return;
    }

    if (current_file_context_.isNull())
        return;

    if (current_file_context_->getState() == FILE_STATE_CHANGED &&
        !main_window_
        ->showFileCloseDialog(current_file_context_->getFileName ()))
        return; // user cancel

    current_file_context_.clear ();
    signal_browser_model_->disconnect (SIGNAL(eventSelected(QSharedPointer<SignalEvent>)));

    // close
    CommandStack::instance().clearStacks();
    signal_browser_model_->saveSettings();

    signal_browser_model_.clear();
    main_window_->setCentralWidget(0);
    tab_widget_ = 0;
    signal_browser_ = 0;
    signal_browser_tab_ = 0;
    file_signal_reader_->close();
    file_signal_reader_ = QSharedPointer<FileSignalReader>(0);
    tab_contexts_.clear ();

    // reset status bar
    main_window_->setStatusBarSignalLength(-1);
    main_window_->setStatusBarNrChannels(-1);

    ApplicationContext::getInstance()->addFileContext (QSharedPointer<FileContext>(0));
    application_context_->setState(APP_STATE_NO_FILE_OPEN);
    main_window_->setWindowTitle (tr("SigViewer"));
}

// file info action
void MainWindowModelImpl::fileInfoAction()
{
    if (!checkMainWindowPtr("fileInfoAction"))
    {
        return;
    }

    // show basic header info dialog
    BasicHeaderInfoDialog basic_header_info_dialog(file_signal_reader_->getBasicHeader(),
                                                   main_window_);

    basic_header_info_dialog.loadSettings();
    basic_header_info_dialog.exec();
    basic_header_info_dialog.saveSettings();
}

// file exit action
void MainWindowModelImpl::fileExitAction()
{
    if (!checkMainWindowPtr("fileExitAction"))
    {
        return;
    }

    if (ApplicationContext::getInstance()->getState() == APP_STATE_FILE_OPEN)
    {
        fileCloseAction();

        if (ApplicationContext::getInstance()->getState()
            == APP_STATE_FILE_OPEN)
        {
            return; // user cancel;
        }
    }

    QApplication::exit();
}

// view zoom in action
void MainWindowModelImpl::viewZoomInAction()
{
    signal_browser_model_->zoomInAll();
}

// view zoom out action
void MainWindowModelImpl::viewZoomOutAction()
{
    signal_browser_model_->zoomOutAll();
}

// view auto scale action
void MainWindowModelImpl::viewAutoScaleAction()
{
    signal_browser_model_->autoScaleAll();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::optionsChannelsAction()
{
    if (signal_browser_model_->setShownChannels (GuiHelper::selectChannels (
            current_file_context_->getChannelManager(), current_file_context_->getFileName(), signal_browser_model_)))
        signal_browser_model_->updateLayout();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::optionsChangeCreationType ()
{
    uint16 current_type = signal_browser_model_->getActualEventCreationType();
    uint16 new_type = GuiHelper::selectEventType (current_type);

    if (new_type != UNDEFINED_EVENT_TYPE)
        signal_browser_model_->setActualEventCreationType (new_type);
}

//-----------------------------------------------------------------------------
// options show events action
void MainWindowModelImpl::optionsShowEventsAction()
{
/*    // set curent shown event types
    EventTypeDialog::IntList shown_event_types;

    signal_browser_model_->getShownEventTypes(shown_event_types);

    EventTypeDialog event_type_dialog(tr("Show Events"),
                                      main_window_);

    event_type_dialog.setShownTypes(shown_event_types);
    event_type_dialog.loadSettings();
    event_type_dialog.exec();
    event_type_dialog.saveSettings();

    if (event_type_dialog.result() == QDialog::Rejected)
    {
        return; // user cancel
    }

    // store colors
    event_type_dialog.storeColors();

    // change shown event types
    event_type_dialog.getShownTypes(shown_event_types);
    QUndoCommand* eventCommand = new SetShownEventTypesViewUndoCommand (*signal_browser_model_, shown_event_types);
    CommandStack::instance().executeViewCommand(eventCommand);*/
}

// options show events action
void MainWindowModelImpl::optionsShowSettingsAction()
{
    SettingsDialog settings_dialog (signal_browser_model_,
                                    signal_browser_model_->getHideableWidgetsVisibilities(),
                                    main_window_);

//    settings_dialog.loadSettings();
    settings_dialog.exec();

    if (settings_dialog.result() == QDialog::Rejected)
    {
        return; // user cancel
    }

    // user ok
//    settings_dialog.saveSettings();

    overflow_detection_ = settings_dialog.isOverflowDetection();

        signal_browser_model_->setHideableWidgetsVisibilities(settings_dialog.getWidgetVisibilities());
        //signal_browser_model_->showChannelLabels(settings_dialog.isShowChannelLables());
        //signal_browser_model_->showXScales(settings_dialog.isShowChannelScales());
        //signal_browser_model_->showYScales(settings_dialog.isShowChannelScales());
        signal_browser_model_->setXGridVisible(settings_dialog.isShowGrid());
        signal_browser_model_->setYGridVisible(settings_dialog.isShowGrid());
        signal_browser_model_->setAutoZoomBehaviour(settings_dialog.getScaleModeType());
        signal_browser_model_->autoScaleAll();
        signal_browser_model_->updateLayout();
}

// help log action
void MainWindowModelImpl::helpLogAction()
{
    if (!checkMainWindowPtr("helpLogAction"))
    {
        return;
    }

    // show log dialog
    LogDialog log_dialog(*log_stream_.get(), main_window_);
    log_dialog.loadSettings();
    log_dialog.exec();
    log_dialog.saveSettings();
}

// help about action
void MainWindowModelImpl::helpAboutAction()
{
    if (!checkMainWindowPtr("helpAboutAction"))
    {
        return;
    }

    main_window_->showHelpAboutDialog();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::storeAndInitTabContext (QSharedPointer<TabContext> context, int tab_index)
{
    tab_contexts_[tab_index] = context;


    application_context_->getGUIActionManager ()->connect (context.data(),
                                                         SIGNAL(selectionStateChanged(TabSelectionState)),
                                                         SLOT(setTabSelectionState(TabSelectionState)));
    application_context_->getGUIActionManager ()->connect (context.data(),
                                                         SIGNAL(editStateChanged(TabEditState)),
                                                         SLOT(setTabEditState(TabEditState)));

    ApplicationContext::getInstance()->setCurrentTabContext (context);

    context->setSelectionState (TAB_STATE_NO_EVENT_SELECTED);
    context->setEditState (TAB_STATE_NO_REDO_NO_UNDO);
}

//-----------------------------------------------------------------------------
// check main window ptr
inline bool MainWindowModelImpl::checkMainWindowPtr(const QString function)
{
    if (!main_window_)
    {
        *log_stream_ << "MainWindowModelImpl::" << function
                     << " Error: MainWindow not set\n";
        return false;
    }

    return true;
}

// set changed
void MainWindowModelImpl::setChanged()
{
    ApplicationContext::getInstance()->getCurrentFileContext()->setState (FILE_STATE_CHANGED);
}

//-----------------------------------------------------------------------------
QSharedPointer<BlocksVisualisationModel> MainWindowModelImpl::createBlocksVisualisationView (QString const& title)
{
    BlocksVisualisationView* bv_view = new BlocksVisualisationView (tab_widget_);
    QSharedPointer<BlocksVisualisationModel> bv_model = QSharedPointer<BlocksVisualisationModel> (new BlocksVisualisationModel (bv_view, 10, channel_manager_->getSampleRate()));

    blocks_visualisation_models_.push_back (bv_model);
    int tab_index = tab_widget_->addTab(bv_view, title);
    browser_models_[tab_index] = bv_model;
    tab_widget_->setCurrentWidget(bv_view);
    QSharedPointer<TabContext> tab_context (new TabContext ());
    storeAndInitTabContext (tab_context, tab_index);

    return bv_model;
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> MainWindowModelImpl::createSignalVisualisationOfFile (QSharedPointer<FileContext> file_ctx)
{
    // waldesel:
    // --begin
    //   to be replaced as soon as multi file support is implemented
    if (!current_file_context_.isNull())
        fileCloseAction();
    // --end
    if (!tab_widget_)
    {
        tab_widget_ = new QTabWidget (main_window_);
        connect (tab_widget_, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
        connect (tab_widget_, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
        tab_widget_->setTabsClosable (true);
    }

    QSharedPointer<TabContext> tab_context (new TabContext);
    file_ctx->setMainTabContext (tab_context);

    QSharedPointer<SignalBrowserModel> model (new SignalBrowserModel (file_ctx->getEventManager(),
                                                                      file_ctx->getChannelManager(),
                                                                      tab_context));
    SignalBrowserView* view = new SignalBrowserView (model, file_ctx->getEventManager(), tab_context, tab_widget_);

    int tab_index = tab_widget_->addTab (view, tr("Signal Data"));
    storeAndInitTabContext (tab_context, tab_index);

    model->setSignalBrowserView (view);
    browser_models_[tab_index] = model;
    model->loadSettings ();

    main_window_->setCentralWidget(tab_widget_);
    tab_widget_->show();
    view->show();

    // waldesel:
    // --begin
    //   this is only to support old code here.. remove this line as soon
    //   command pattern for gui commands is finalised
    signal_browser_model_ = model;
    signal_browser_ = view;
    current_file_context_ = file_ctx;
    event_manager_ = file_ctx->getEventManager();
    channel_manager_ = file_ctx->getChannelManager();
    file_signal_reader_ = file_ctx->getFileSignalReader();
    // --end


    recent_file_list_.removeAll (file_ctx->getFilePathAndName());
    if (recent_file_list_.size() == NUMBER_RECENT_FILES_)
        recent_file_list_.pop_back();
    recent_file_list_.push_front (file_ctx->getFilePathAndName());


    // waldesel:
    // --begin
    //   to be replaced as soon as new zooming is implemented
    main_window_->setStatusBarSignalLength (file_ctx->getChannelManager()->getDurationInSec());
    // --end

    main_window_->setStatusBarNrChannels (file_ctx->getChannelManager()->getNumberChannels());
    main_window_->setWindowTitle (file_ctx->getFileName() + tr(" - SigViewer"));

    model->connect (file_ctx->getEventManager().data(), SIGNAL(eventCreated(QSharedPointer<SignalEvent const>)),
                                   SLOT(addEventItem(QSharedPointer<SignalEvent const>)));
    model->connect (file_ctx->getEventManager().data(), SIGNAL(eventRemoved(EventID)),
                                   SLOT(removeEventItem(EventID)));
    model->connect (file_ctx->getEventManager().data(), SIGNAL(eventChanged(EventID)),
                                   SLOT(updateEvent(EventID)));


    return model;
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::closeCurrentFileTabs ()
{
    // waldesel:
    // --begin
    //   this is only to support old code here.. refactor these lines as soon
    //   command pattern for gui commands is finalised
    signal_browser_model_.clear ();
    current_file_context_.clear ();
    event_manager_.clear ();
    channel_manager_.clear ();
    file_signal_reader_.clear ();
    CommandStack::instance().clearStacks();
    // --end

    // waldesel:
    // --begin
    //   to be refactored as soon as multi file support is implemented
    main_window_->setCentralWidget (0);
    tab_widget_ = 0;
    signal_browser_ = 0;
    signal_browser_tab_ = 0;
    tab_contexts_.clear ();
    // --end

    main_window_->setStatusBarSignalLength(-1);
    main_window_->setStatusBarNrChannels(-1);
    main_window_->setWindowTitle (tr("SigViewer"));
}

//-----------------------------------------------------------------------------
QSharedPointer<SignalVisualisationModel> MainWindowModelImpl::getCurrentSignalVisualisationModel ()
{
    if (!tab_widget_)
        return QSharedPointer<SignalVisualisationModel>(0);

    return browser_models_[tab_widget_->currentIndex()];
}

} // namespace BioSig_
