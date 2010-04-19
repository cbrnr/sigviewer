// main_window_model.cpp

#include "main_window_model_impl.h"
#include "main_window.h"
#include "tab_context.h"
#include "file_context.h"
#include "application_context.h"
#include "gui_action_manager.h"

#include "file_handling/file_signal_reader_factory.h"
#include "file_handling/file_signal_writer_factory.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "gui_impl/gui_helper_functions.h"
#include "base/signal_event.h"
#include "basic_header_info_dialog.h"
#include "log_dialog.h"
#include "gui_impl/channel_selection_dialog.h"
#include "event_time_selection_dialog.h"
#include "go_to_dialog.h"
#include "event_type_dialog.h"
#include "event_table_dialog.h"
#include "settings_dialog.h"
#include "command_stack.h"

#include "abstract_browser_model.h"

#include "signal_browser/signal_browser_model_4.h"
#include "signal_browser/signal_browser_view.h"
#include "signal_browser/delete_event_undo_command.h"
#include "signal_browser/calculate_event_mean_command.h"
#include "signal_browser/calculcate_frequency_spectrum_command.h"
#include "signal_browser/new_event_undo_command.h"
#include "next_event_view_undo_command.h"
#include "set_shown_event_types_view_undo_command.h"
#include "fit_view_to_event_view_undo_command.h"
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

unsigned const MainWindowModelImpl::NUMBER_RECENT_FILES_ = 8;


// constructor
MainWindowModelImpl::MainWindowModelImpl ()
: main_window_(0),
  application_context_ (ApplicationContext::getInstance()),
  signal_browser_model_ (0),
  signal_browser_ (0),
  tab_widget_ (0),
  secs_per_page_("10"),
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

    secs_per_page_ = settings.value("secs_per_page", secs_per_page_).toString();
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
    settings.setValue("secs_per_page", secs_per_page_);
    settings.setValue("overflow_detection", overflow_detection_);
    settings.endGroup();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::tabChanged (int tab_index)
{
    if (tab_contexts_.find(tab_index) != tab_contexts_.end ())
        tab_contexts_[tab_index]->gotActive ();
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
void MainWindowModelImpl::calculateERDERSMap ()
{
    if (signal_browser_model_.isNull())
        return;

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


//-----------------------------------------------------------------------------
// file open action
void MainWindowModelImpl::fileOpenAction()
{
    if (!checkMainWindowPtr("fileOpenAction"))
    {
        return;
    }

    // open dialog
    QSettings settings("SigViewer");
    QString path = settings.value("MainWindowModelImpl/file_open_path", ".").toString();
    QString extensions = FileSignalReaderFactory::getInstance()->getExtensions();
    QString file_name;
    file_name = main_window_->showOpenDialog(path, extensions);

    if (file_name.isEmpty())
    {
        return; // user cancel
    }

    openFile(file_name);
}

// file save action
void MainWindowModelImpl::fileSaveAction()
{
    if (!checkMainWindowPtr("fileSaveAction"))
    {
        return;
    }

    // get writer for file
    FileSignalWriter* file_signal_writer = 0;

    QString file_name = current_file_context_->getFilePathAndName();

    if (file_name.lastIndexOf('.') != -1)
    {
        file_signal_writer = FileSignalWriterFactory::getInstance()->
                    getElement(file_name.mid(file_name.lastIndexOf('.')));
    }

    if (!file_signal_writer)
    {
        *log_stream_ << "MainWindowModelImpl::fileSaveAction Error: "
                     << "not suported file format: '"
                     << file_name.mid(file_name.lastIndexOf('.')) << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName());
        return;
    }

    // get events from buffer
    FileSignalReader::SignalEventVector event_vector;
    QList<EventID> events = event_manager_->getAllEvents ();
    for (QList<EventID>::iterator events_iter = events.begin();
         events_iter != events.end();
         ++events_iter)
        event_vector.push_back (*event_manager_->getEvent (*events_iter));

    // save
    file_signal_writer->setLogStream(log_stream_.get());
    QString save_error = file_signal_writer->save(*file_signal_reader_, event_vector,
                                                  file_name, true);

    if (save_error.size() == 0)
        ApplicationContext::getInstance()->getCurrentFileContext()->setState (FILE_STATE_UNCHANGED);
    else
    {
        *log_stream_ << "MainWindowModelImpl::fileSaveAction Error: writing file :'"
                     << file_name << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName() + ": \"" + save_error + "\"");
    }

    delete file_signal_writer;
}

// file save as action

void MainWindowModelImpl::fileSaveAsAction()
{
    if (!checkMainWindowPtr("fileSaveAsAction"))
    {
        return;
    }

    // save as dialog
    QSettings settings("SigViewer");
    QString path = settings.value("MainWindowModelImpl/file_open_path", ".").toString();
    QString extensions = FileSignalWriterFactory::getInstance()->getExtensions();
    QString file_name;

    file_name = main_window_->showSaveAsDialog(path, extensions);

    if (file_name.isEmpty() || (QFile::exists(file_name) &&
        !main_window_->showOverwriteDialog(file_name)))
    {
        return; // user cancel
    }

    // get writer for file
    FileSignalWriter* file_signal_writer = 0;

    if (file_name.lastIndexOf('.') != -1)
    {
        file_signal_writer = FileSignalWriterFactory::getInstance()
                ->getElement(file_name.mid(file_name.lastIndexOf('.')));
    }

    if (!file_signal_writer)
    {
        *log_stream_ << "MainWindowModelImpl::fileSaveAsAction Error: "
                     << "not suported file format: '"
                     << file_name.mid(file_name.lastIndexOf('.')) << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName());
        return;
    }

    // get events from buffer
    FileSignalReader::SignalEventVector event_vector;
    QList<EventID> events = event_manager_->getAllEvents ();
    for (QList<EventID>::iterator events_iter = events.begin();
         events_iter != events.end();
         ++events_iter)
        event_vector.push_back (*event_manager_->getEvent (*events_iter));

    // save
    file_signal_writer->setLogStream(log_stream_.get());

    QString save_error = file_signal_writer->save(*file_signal_reader_, event_vector,
                                                  file_name, true);

    if (save_error.size () == 0)
    {
        file_signal_reader_->close();
        file_signal_reader_->open(file_name, overflow_detection_);
        ApplicationContext::getInstance()->getCurrentFileContext()->setState (FILE_STATE_UNCHANGED);
    }
    else
    {
        *log_stream_ << "MainWindowModelImpl::fileSaveAsAction "
                     << "Error: writing file:'" << file_name << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName() + ": \"" + tr(save_error.toLatin1()) + "\"");
    }

    delete file_signal_writer;
}

// file export events action
void MainWindowModelImpl::fileExportEventsAction()
{
    if (!checkMainWindowPtr("fileExportEventsAction"))
    {
        return;
    }

    // get events
    FileSignalReader::SignalEventVector event_vector;
    QList<EventID> events = event_manager_->getAllEvents ();
    for (QList<EventID>::iterator events_iter = events.begin();
         events_iter != events.end();
         ++events_iter)
        event_vector.push_back (*event_manager_->getEvent (*events_iter));

    // get event types
    SignalBrowserModel::IntList event_types;
    FileSignalReader::SignalEventVector::iterator it;

    for (it = event_vector.begin(); it != event_vector.end(); it++)
    {
        event_types << it->getType();
    }

    qSort(event_types);

    // event type dialog
    EventTypeDialog event_type_dialog(tr("Export Events"),
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

    // export dialog
    QString file_name = file_signal_reader_->getBasicHeader()->getFileName();
    QSettings settings("SigViewer");

    file_name = settings.value("MainWindowModelImpl/file_open_path", ".").toString() + "/" +
                file_name.left(file_name.lastIndexOf('.'));

    QString extensions = FileSignalWriterFactory::getInstance()->getExtensions();
    file_name = main_window_->showExportDialog(file_name, extensions);

    if (file_name.isEmpty() || (QFile::exists(file_name) &&
        !main_window_->showOverwriteDialog(file_name)))
    {
        return; // user cancel
    }

    // get writer for file
    FileSignalWriter* file_signal_writer = 0;

    if (file_name.lastIndexOf('.') != -1)
    {
        file_signal_writer = FileSignalWriterFactory::getInstance()
                    ->getElement(file_name.mid(file_name.lastIndexOf('.')));
    }

    if (!file_signal_writer)
    {
        *log_stream_ << "MainWindowModelImpl::fileExportEventsAction Error: "
                     << "not suported file format: '"
                     << file_name.mid(file_name.lastIndexOf('.')) << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName());
        return;
    }

    // remove not selected events from vector
    it = event_vector.begin();
    while(it != event_vector.end())
    {
        if (!event_types.contains(it->getType()))
        {
            it = event_vector.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // export
    file_signal_writer->setLogStream(log_stream_.get());

    QString save_error = file_signal_writer->save(*file_signal_reader_, event_vector,
                                                  file_name, false);

    if (save_error.size () == 0)
    {
        //setState(STATE_FILE_OPENED);
    }
    else
    {
        *log_stream_ << "MainWindowModelImpl::fileExportEventsAction "
                     << "Error: writing file:'" << file_name << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName() + ": \""+ tr(save_error.toLatin1()) + "\"");
    }

    delete file_signal_writer;
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

    int32 number_channels = file_signal_reader_->getBasicHeader()->getNumberChannels();

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

    openFile(recent_file_action->text());
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::openFile (QString const& file_path)
{
    // close
    if (ApplicationContext::getInstance()->getState() == APP_STATE_FILE_OPEN)
    {
        fileCloseAction();
        if (application_context_->getState() != APP_STATE_NO_FILE_OPEN)
            return; // user cancel
    }

    file_signal_reader_ = createAndOpenFileSignalReader (file_path);
    if (file_signal_reader_.isNull())
        return;

    QString file_name = file_path.section (QDir::separator(), -1);

    QSharedPointer<TabContext> tab_context (new TabContext ());
    event_manager_ = QSharedPointer<EventManagerImpl>(new EventManagerImpl (file_signal_reader_));
    channel_manager_ = QSharedPointer<ChannelManager>(new ChannelManagerImpl (file_signal_reader_));
    QSharedPointer<FileContext> file_context (new FileContext (file_path, event_manager_, channel_manager_, file_signal_reader_));
    ApplicationContext::getInstance()->addFileContext (file_context);
    current_file_context_ = file_context;
    application_context_->getGUIActionManager()->connect (file_context.data (), SIGNAL(stateChanged(FileState)), SLOT(setFileState(FileState)));


    int32 sep_pos = file_path.lastIndexOf(DIR_SEPARATOR);
    QString path = sep_pos == -1 ? "." : file_path.mid(0, sep_pos);
    QSettings settings("SigViewer");
    settings.setValue("MainWindowModelImpl/file_open_path", path);


    // initialize signal browser
    signal_browser_model_ = QSharedPointer<SignalBrowserModel> (new SignalBrowserModel(file_context, tab_context));

    connect (event_manager_.data(), SIGNAL(eventCreated(QSharedPointer<SignalEvent const>)),
             signal_browser_model_.data(), SLOT(addEventItem(QSharedPointer<SignalEvent const>)));
    connect (event_manager_.data(), SIGNAL(eventRemoved(EventID)),
             signal_browser_model_.data(), SLOT(removeEventItem(EventID)));
    connect (event_manager_.data(), SIGNAL(eventChanged(EventID)),
             signal_browser_model_.data(), SLOT(setEventChanged(EventID)));



    if (!tab_widget_)
    {
        tab_widget_ = new QTabWidget (main_window_);
        connect (tab_widget_, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
        connect (tab_widget_, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
        tab_widget_->setTabsClosable (true);
    }

    signal_browser_ = new SignalBrowserView (signal_browser_model_, event_manager_, tab_context, tab_widget_);
    signal_browser_model_->setSignalBrowserView(signal_browser_);
    signal_browser_model_->loadSettings();

    int tab_index = tab_widget_->addTab (signal_browser_, tr("Signal Data"));
    storeAndInitTabContext (tab_context, tab_index);
    browser_models_[tab_index] = signal_browser_model_;

    tab_widget_->hide();

    main_window_->setCentralWidget(tab_widget_);


    application_context_->setState(APP_STATE_FILE_OPEN);

    // update recent files
    recent_file_list_.removeAll (file_path);
    if (recent_file_list_.size() == NUMBER_RECENT_FILES_)
    {
        recent_file_list_.pop_back();
    }

    recent_file_list_.push_front (file_path);

    // select channels
    std::set<ChannelID> shown_channels = GuiHelper::selectChannels (
            current_file_context_->getChannelManager(), file_name, signal_browser_model_);
    if (!shown_channels.size ())
    {
        fileCloseAction ();
        return;
    }

    tab_widget_->show();
    signal_browser_->show();

    signal_browser_model_->setShownChannels (shown_channels);
    main_window_->setSignalsPerPage(-1); // all

    // set status bar
    main_window_->setStatusBarSignalLength(file_signal_reader_->getBasicHeader()->getNumberRecords() *
                                           file_signal_reader_->getBasicHeader()->getRecordDuration());

    main_window_->setStatusBarNrChannels(file_signal_reader_->getBasicHeader()->getNumberChannels());

    secsPerPageChanged (secs_per_page_);
    main_window_->setSecsPerPage(secs_per_page_);
    signal_browser_model_->updateLayout();
    main_window_->setWindowTitle (file_name + tr(" - SigViewer"));
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

// edit to all channels action
void MainWindowModelImpl::editToAllChannelsAction()
{
    if (!checkMainWindowPtr("editToAllChannelsAction"))
    {
        return;
    }

    signal_browser_model_->setSelectedEventToAllChannels();
}

// edit copy to channles action
void MainWindowModelImpl::editCopyToChannelsAction()
{
    if (!checkMainWindowPtr("editCopyToChannelsAction"))
    {
        return;
    }

    signal_browser_model_->copySelectedEventToChannels();
}

// edit delete action
void MainWindowModelImpl::editDeleteAction()
{
    if (!checkMainWindowPtr("editDeleteAction"))
    {
        return;
    }

    QUndoCommand* deleteCommand = new DeleteEventUndoCommand (*event_manager_, signal_browser_model_->getSelectedEventItem()->getId ());
    tab_contexts_[tab_widget_->currentIndex()]->executeCommand (deleteCommand);
}

// edit change channel action
void MainWindowModelImpl::editChangeChannelAction()
{
    if (!checkMainWindowPtr("editChangeChannelAction"))
    {
        return;
    }

    signal_browser_model_->changeSelectedEventChannel();
}

// edit change type action
void MainWindowModelImpl::editChangeTypeAction()
{
    if (!checkMainWindowPtr("editChangeTypeAction"))
    {
        return;
    }

    uint16 current_type = 0;
    if (signal_browser_model_->getSelectedEventItem())
        current_type = signal_browser_model_->getSelectedSignalEvent()->getType();

    uint16 new_type = selectEventTypeDialog (current_type);

    if (new_type != static_cast<uint16>(-1))
        signal_browser_model_->changeSelectedEventType (new_type);
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::editEventTableAction ()
{
    EventTableDialog event_table_dialog (*event_manager_,
        ApplicationContext::getInstance()->getCurrentFileContext()->getMainTabContext(),
                                        file_signal_reader_->getBasicHeader(),
                                        main_window_);
    event_table_dialog.loadSettings();
    event_table_dialog.exec();
    event_table_dialog.saveSettings();
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::editInsertOverAction ()
{
    uint16 event_type = signal_browser_model_->getActualEventCreationType();
    QSharedPointer<SignalEvent> new_event = QSharedPointer<SignalEvent>(new SignalEvent(*(signal_browser_model_->getSelectedSignalEvent().data())));
    new_event->setType (event_type);
    NewEventUndoCommand* new_event_command = new NewEventUndoCommand (event_manager_, new_event);
    tab_contexts_[tab_widget_->currentIndex()]->executeCommand (new_event_command);
}


// view zoom in action
void MainWindowModelImpl::viewZoomInAction()
{
    signal_browser_model_->zoomInAll();
}

// mouse mode new action
void MainWindowModelImpl::mouseModeNewAction()
{
    signal_browser_model_->unselectEvent();
    signal_browser_model_->setMode(MODE_NEW);
    signal_browser_->setScrollMode(false);
}

// mouse mode pointer action
void MainWindowModelImpl::mouseModePointerAction()
{
    signal_browser_model_->setMode(MODE_POINTER);
    signal_browser_->setScrollMode(false);
}

// mouse mode hand action
void MainWindowModelImpl::mouseModeHandAction()
{
    signal_browser_model_->setMode(MODE_HAND);
    signal_browser_->setScrollMode(true);
}

// mouse mode shift signal action
void MainWindowModelImpl::mouseModeShiftSignalAction()
{
    signal_browser_model_->setMode(MODE_SHIFT_SIGNAL);
    signal_browser_->setScrollMode(false);
}

// mouse mode zoom action
//void MainWindowModelImpl::mouseModeZoomAction()
//{
//    if (!checkMainWindowPtr("mouseModeZoomAction") ||
//        !checkNotClosedState("mouseModeZoomAction"))
//    {
//        return;
//    }
//
//    signal_browser_model_->setMode(SignalBrowserModel::MODE_ZOOM);
//    signal_browser_->setScrollMode(false);
//}

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

// view go to action
void MainWindowModelImpl::viewGoToAction()
{
    GoToDialog go_to_dialog(file_signal_reader_->getBasicHeader(), main_window_);

    // current selected channels
    for (uint32 channel_nr = 0;
         channel_nr < file_signal_reader_->getBasicHeader()->getNumberChannels();
         channel_nr++)
    {
        if (signal_browser_model_->isChannelShown(channel_nr))
        {
            go_to_dialog.setChannelShown(channel_nr);
        }
    }

    go_to_dialog.loadSettings();
    go_to_dialog.exec();
    go_to_dialog.saveSettings();

    if (go_to_dialog.result() == QDialog::Rejected)
    {
        return; // user cancel
    }

    signal_browser_model_->goTo(go_to_dialog.getSecond(),

                                go_to_dialog.getChannelIndex());
}

//-------------------------------------------------------------------
// view select next event action
void MainWindowModelImpl::viewShowAndSelectNextEventAction()
{
    QUndoCommand* eventCommand = new NextEventViewUndoCommand (*signal_browser_model_);
    CommandStack::instance().executeViewCommand(eventCommand);
}

//-------------------------------------------------------------------
// view select previous event action
void MainWindowModelImpl::viewShowAndSelectPreviousEventAction()
{
    QUndoCommand* eventCommand = new NextEventViewUndoCommand (*signal_browser_model_, false);
    CommandStack::instance().executeViewCommand(eventCommand);
}

//-------------------------------------------------------------------
// view select next event action
void MainWindowModelImpl::viewShowEventsOfSelectedTypeAction()
{
    SignalBrowserModel::IntList shown_event_types;
    shown_event_types.append (signal_browser_model_->getSelectedSignalEvent()->getType());

    QUndoCommand* eventCommand = new SetShownEventTypesViewUndoCommand (*signal_browser_model_, shown_event_types);
    CommandStack::instance().executeViewCommand(eventCommand);
}



//-------------------------------------------------------------------
// fits the view to the selected event action
void MainWindowModelImpl::viewFitToEventAction()
{
    QUndoCommand* eventCommand = new FitViewToEventViewUndoCommand (*signal_browser_model_);
    CommandStack::instance().executeViewCommand(eventCommand);
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
    uint16 new_type = selectEventTypeDialog (current_type);

    if (new_type != static_cast<uint16>(-1))
        signal_browser_model_->setActualEventCreationType (new_type);
}

//-----------------------------------------------------------------------------
uint16 MainWindowModelImpl::selectEventTypeDialog (uint16 preselected_type) const
{
    uint16 new_type = -1;

    std::set<uint16> types = signal_browser_model_->getShownEventTypes();
    QStringList event_type_list;
    int32 current_item = 0;

    for (std::set<uint16>::const_iterator it = types.begin();
         it != types.end();
         it++)
    {
        if (preselected_type == *it)
            current_item = event_type_list.size();

        QString event_name
                = ApplicationContext::getInstance()->getEventTableFileReader()->getEventName(*it);

            event_type_list.append(event_name + " " + QString("(%1)")
                                                        .arg(*it,4, 16)
                                                        .replace(' ', '0'));
    }

    // dialog
    bool ok = false;

    QString res = QInputDialog::getItem(main_window_, tr("Change Type"),
                                        tr("Select new Type:"),
                                        event_type_list, current_item,
                                        false, &ok);

    if (ok)
        new_type = res.right(5).left(4).toUShort(0, 16);

    return new_type;
}

//-----------------------------------------------------------------------------
// options show events action
void MainWindowModelImpl::optionsShowEventsAction()
{
    // set curent shown event types
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
    CommandStack::instance().executeViewCommand(eventCommand);
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

    context->setSelectionState (TAB_STATE_NO_EVENT_SELECTED);
    context->setEditState (TAB_STATE_NO_REDO_NO_UNDO);
}

//-----------------------------------------------------------------------------
QSharedPointer<FileSignalReader> MainWindowModelImpl::createAndOpenFileSignalReader
        (QString const& file_path) const
{
    QSharedPointer<FileSignalReader> signal_reader;
    QString load;

    if (file_path.lastIndexOf('.') != -1)
    {
        signal_reader = QSharedPointer<FileSignalReader>(FileSignalReaderFactory::getInstance()->
                    getElement (file_path.mid (file_path.lastIndexOf('.'))));
        if (signal_reader)
        {
            signal_reader->setLogStream(log_stream_.get());
            load = signal_reader->open (file_path, overflow_detection_);

            if (load.size ())
                main_window_->showErrorReadDialog (file_path + " \"" + load + "\"");
            else
                return signal_reader;
        }
    }
    return QSharedPointer<FileSignalReader> (0);
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

// secs per page changed
void MainWindowModelImpl::secsPerPageChanged(const QString& secs_per_page)
{
    bool ok = false;
    float64 f_secs_per_page = secs_per_page.toFloat(&ok);
    float64 pixel_per_sec;

    if (ok)
    {
        f_secs_per_page = (int32)(f_secs_per_page * 10) / 10.0;
        pixel_per_sec = signal_browser_->getVisibleWidth() /
                        f_secs_per_page;
    }
    else
    {
        // whole
        pixel_per_sec = signal_browser_->getVisibleWidth() /
                        (file_signal_reader_->getBasicHeader()->getNumberRecords() *
                         file_signal_reader_->getBasicHeader()->getRecordDuration());
    }
    browser_models_[tab_widget_->currentIndex()]->setPixelPerXUnit (pixel_per_sec);
    browser_models_[tab_widget_->currentIndex()]->updateLayout ();

    secs_per_page_ = secs_per_page;
}

// signals per page changed
void MainWindowModelImpl::signalsPerPageChanged(const QString& signals_per_page)
{
    bool ok = false;
    float64 tmp = signals_per_page.toFloat(&ok);
    if (!ok)
    {
        // all channels
        tmp = signal_browser_model_->getNumberShownChannels();

        tmp = tmp == 0 ? 1 : tmp;
    }

    int32 signal_height;
    // TODO: move this calculation into browser-model!!!
    signal_height = (int32)(signal_browser_->getVisibleHeight() / tmp) -
                    signal_browser_model_->getSignalSpacing();
    browser_models_[tab_widget_->currentIndex()]->setItemsHeight (signal_height);
    browser_models_[tab_widget_->currentIndex()]->updateLayout ();
}

// pixel per sec changed
void MainWindowModelImpl::pixelPerSecChanged(float64 pixel_per_sec)
{
    float64 secs_per_page = signal_browser_->getVisibleWidth() /
                            pixel_per_sec;

    secs_per_page = (int32)(secs_per_page * 10) / 10.0;
    main_window_->setSecsPerPage(secs_per_page);
}

//-----------------------------------------------------------------------------
void MainWindowModelImpl::signalHeightChanged(int32 signal_height)
{
    float64 signals_per_page;
    signals_per_page = signal_browser_->getVisibleHeight() /
                       (float64)(signal_height +
                                 signal_browser_model_->getSignalSpacing());

    signals_per_page = (int32)(signals_per_page * 10) / 10.0;
    main_window_->setSignalsPerPage(signals_per_page);
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
    QSharedPointer<BlocksVisualisationModel> bv_model = QSharedPointer<BlocksVisualisationModel> (new BlocksVisualisationModel (bv_view, signal_browser_model_->getPixelPerXUnit (), channel_manager_->getSampleRate()));

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

    QSharedPointer<SignalBrowserModel> model (new SignalBrowserModel (file_ctx, tab_context));
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
    secsPerPageChanged (secs_per_page_);
    main_window_->setSecsPerPage (secs_per_page_);
    // --end

    main_window_->setStatusBarNrChannels (file_ctx->getChannelManager()->getNumberChannels());
    main_window_->setWindowTitle (file_ctx->getFileName() + tr(" - SigViewer"));

    model->connect (file_ctx->getEventManager().data(), SIGNAL(eventCreated(QSharedPointer<SignalEvent const>)),
                                   SLOT(addEventItem(QSharedPointer<SignalEvent const>)));
    model->connect (file_ctx->getEventManager().data(), SIGNAL(eventRemoved(EventID)),
                                   SLOT(removeEventItem(EventID)));
    model->connect (file_ctx->getEventManager().data(), SIGNAL(eventChanged(EventID)),
                                   SLOT(setEventChanged(EventID)));


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


} // namespace BioSig_

