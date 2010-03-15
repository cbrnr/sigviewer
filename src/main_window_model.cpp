// main_window_model.cpp

#include "main_window_model.h"
#include "main_window.h"
#include "tab_context.h"
#include "file_context.h"
#include "application_context.h"
#include "gui_action_manager.h"

#include "base/file_signal_reader_factory.h"
#include "base/file_signal_writer_factory.h"
#include "base/event_table_file_reader.h"
#include "base/signal_event.h"
#include "basic_header_info_dialog.h"
#include "log_dialog.h"
#include "channel_selection_dialog.h"
#include "event_time_selection_dialog.h"
#include "go_to_dialog.h"
#include "event_type_dialog.h"
#include "gui_signal_buffer.h"
#include "event_table_dialog.h"
#include "event_color_manager.h"
#include "settings_dialog.h"
#include "command_stack.h"

#include "abstract_browser_model.h"

#include "signal_browser/signal_browser_model_4.h"
#include "signal_browser/signal_browser_view.h"
#include "signal_browser/delete_event_undo_command.h"
#include "signal_browser/calculate_event_mean_command.h"
#include "signal_browser/calculcate_frequency_spectrum_command.h"
#include "next_event_view_undo_command.h"
#include "set_shown_event_types_view_undo_command.h"
#include "fit_view_to_event_view_undo_command.h"
#include "block_visualisation/blocks_visualisation_view.h"
#include "block_visualisation/blocks_visualisation_model.h"


#include <QString>
#include <QApplication>
#include <QFile>
#include <QAction>
#include <QTextStream>
#include <QSettings>
#include <QSharedPointer>
#include <QObject>

#include <cmath>
#include <iostream>

namespace BioSig_
{

// constructor
MainWindowModel::MainWindowModel (ApplicationContext& application_context)
: main_window_(0),
  application_context_ (application_context),
  selection_state_(SELECTION_STATE_NONE),
  signal_browser_model_ (0),
  signal_browser_ (0),
  tab_widget_ (0),
  number_recent_files_(8),
  secs_per_page_("10"),
  overflow_detection_(false),
  file_context_ (0)
{
    log_stream_.reset(new QTextStream(&log_string_));
    file_signal_reader_.reset(0);

    event_table_file_reader_ = QSharedPointer<EventTableFileReader>(new EventTableFileReader);
    event_table_file_reader_->setLogStream(log_stream_.get());
    event_color_manager_
        .reset(new EventColorManager(*(event_table_file_reader_.data())));
}

// destructor
MainWindowModel::~MainWindowModel()
{
    // nothing
}

// get log stream
QTextStream& MainWindowModel::getLogStream()
{
    return *log_stream_.get();
}

// get event table file reader
QSharedPointer<EventTableFileReader> MainWindowModel::getEventTableFileReader()
{
    return event_table_file_reader_;
}

// get event color manager
EventColorManager& MainWindowModel::getEventColorManager()
{
    return *event_color_manager_.get();
}

// set main window
void MainWindowModel::setMainWindow(MainWindow* main_window)
{
    main_window_ = main_window;
    application_context_.setState(APP_STATE_NO_FILE_OPEN);
}

// void load settings
void MainWindowModel::loadSettings()
{
    if (checkMainWindowPtr("loadSettings"))
    {
        main_window_->loadSettings();
    }

    event_color_manager_->loadSettings();

    QSettings settings("SigViewer");
    settings.beginGroup("MainWindowModel");

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
void MainWindowModel::saveSettings()
{
    if (checkMainWindowPtr("saveSettings"))
    {
        main_window_->saveSettings();
    }

    event_color_manager_->saveSettings();

    QSettings settings("SigViewer");
    settings.beginGroup("MainWindowModel");
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

// set selection state
void MainWindowModel::setSelectionState(SelectionState selection_state)
{
    selection_state_ = selection_state;

    switch(selection_state_)
    {
        case SELECTION_STATE_OFF:
//            main_window_->setEditToAllChannelsEnabled(false);
//            main_window_->setEditCopyToChannelsEnabled(false);
//            main_window_->setEditDeleteEnabled(false);
//            main_window_->setEditChangeChannelEnabled(false);
//            main_window_->setEditChangeTypeEnabled(false);
            main_window_->setViewGoToNextPreviousEventEnabled(false);
            main_window_->setViewFitToEvent(false);
            main_window_->setViewHideEventsOfOtherType(false);

            break;

        case SELECTION_STATE_NONE:
//            main_window_->setEditToAllChannelsEnabled(false);
//            main_window_->setEditCopyToChannelsEnabled(false);
//            main_window_->setEditDeleteEnabled(false);
//            main_window_->setEditChangeChannelEnabled(false);
//            main_window_->setEditChangeTypeEnabled(true);
            main_window_->setViewGoToNextPreviousEventEnabled(false);
            main_window_->setViewFitToEvent(false);
            main_window_->setViewHideEventsOfOtherType(false);

            break;

        case SELECTION_STATE_ONE_CHANNEL:
//            main_window_->setEditToAllChannelsEnabled(true);
//            main_window_->setEditCopyToChannelsEnabled(true);
//            main_window_->setEditDeleteEnabled(true);
//            main_window_->setEditChangeChannelEnabled(true);
//            main_window_->setEditChangeTypeEnabled(true);
            main_window_->setViewGoToNextPreviousEventEnabled(true);
            main_window_->setViewFitToEvent(true);
            main_window_->setViewHideEventsOfOtherType(true);

            break;

        case SELECTION_STATE_ALL_CHANNELS:
//            main_window_->setEditToAllChannelsEnabled(false);
//            main_window_->setEditCopyToChannelsEnabled(false);
//            main_window_->setEditDeleteEnabled(true);
//            main_window_->setEditChangeChannelEnabled(true);
//            main_window_->setEditChangeTypeEnabled(true);
            main_window_->setViewGoToNextPreviousEventEnabled(true);
            main_window_->setViewFitToEvent(true);
            main_window_->setViewHideEventsOfOtherType(true);

            break;
    }

}

//-----------------------------------------------------------------------------
void MainWindowModel::tabChanged (int)
{
    // TODO: implement!!!
}

//-----------------------------------------------------------------------------
void MainWindowModel::closeTab (int tab_index)
{
    if (tab_index == 0)
        return;
    QWidget* widget = tab_widget_->widget (tab_index);
    browser_models_.erase (tab_index);
    tab_widget_->removeTab (tab_index);
    delete widget;
}

//-----------------------------------------------------------------------------
void MainWindowModel::calculateMeanAction ()
{
    if (signal_browser_model_.isNull())
        return;
    std::map<uint32, QString> shown_channels = signal_browser_model_->getShownChannels ();
    std::set<uint16> displayed_event_types = signal_browser_model_->getDisplayedEventTypes ();
    std::map<uint16, QString> shown_event_types;
    for (std::set<uint16>::iterator event_type_it = displayed_event_types.begin();
         event_type_it != displayed_event_types.end();
         ++event_type_it)
    {
        shown_event_types[*event_type_it] = event_table_file_reader_->getEventName (*event_type_it);
    }

    EventTimeSelectionDialog event_time_dialog (shown_event_types, shown_channels,
                                                signal_browser_model_->getSignalBuffer());
    if (event_time_dialog.exec () == QDialog::Rejected)
        return;
    else
    {
        CalculateEventMeanCommand command (signal_browser_model_, *this,
                                           event_time_dialog.getSelectedEventType(),
                                           event_time_dialog.getSelectedChannels(),
                                           event_time_dialog.getSecondsBeforeEvent(),
                                           event_time_dialog.getLengthInSeconds());
        command.execute();
    }
}

//-----------------------------------------------------------------------------
void MainWindowModel::calculateFrequencySpectrumAction ()
{
    if (signal_browser_model_.isNull())
        return;
    std::map<uint32, QString> shown_channels = signal_browser_model_->getShownChannels ();
    std::set<uint16> displayed_event_types = signal_browser_model_->getDisplayedEventTypes ();
    std::map<uint16, QString> shown_event_types;
    for (std::set<uint16>::iterator event_type_it = displayed_event_types.begin();
         event_type_it != displayed_event_types.end();
         ++event_type_it)
    {
        shown_event_types[*event_type_it] = event_table_file_reader_->getEventName (*event_type_it);
    }

    EventTimeSelectionDialog event_time_dialog (shown_event_types, shown_channels,
                                                signal_browser_model_->getSignalBuffer());
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
void MainWindowModel::calculateERDERSMap ()
{
    if (signal_browser_model_.isNull())
        return;

}


//-----------------------------------------------------------------------------
void MainWindowModel::undoViewAction()
{
    CommandStack::instance().undoLastViewCommand();
}

//-----------------------------------------------------------------------------
void MainWindowModel::redoViewAction()
{
    CommandStack::instance().redoLastUndoneViewCommand();
}

//-----------------------------------------------------------------------------
void MainWindowModel::undoAction()
{
    CommandStack::instance().undoLastEditCommand();
    //if (state_ == STATE_FILE_OPENED)
    //    setState(STATE_FILE_CHANGED);
}

//-----------------------------------------------------------------------------
void MainWindowModel::redoAction()
{
    CommandStack::instance().redoLastUndoneEditCommand();;
}


//-----------------------------------------------------------------------------
// file open action
void MainWindowModel::fileOpenAction()
{
    if (!checkMainWindowPtr("fileOpenAction"))
    {
        return;
    }

    // open dialog
    QSettings settings("SigViewer");
    QString path = settings.value("MainWindowModel/file_open_path", ".").toString();
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
void MainWindowModel::fileSaveAction()
{
    if (!checkMainWindowPtr("fileSaveAction") ||
        !file_context_)
    {
        return;
    }

    // get writer for file
    FileSignalWriter* file_signal_writer = 0;

    QString file_name = file_signal_reader_->getBasicHeader()->getFullFileName();

    if (file_name.lastIndexOf('.') != -1)
    {
        file_signal_writer = FileSignalWriterFactory::getInstance()->
                    getElement(file_name.mid(file_name.lastIndexOf('.')));
    }

    if (!file_signal_writer)
    {
        *log_stream_ << "MainWindowModel::fileSaveAction Error: "
                     << "not suported file format: '"
                     << file_name.mid(file_name.lastIndexOf('.')) << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName());
        return;
    }

    // get events from buffer
    FileSignalReader::SignalEventVector event_vector;
    signal_browser_model_->getEvents(event_vector);

    // save
    file_signal_writer->setLogStream(log_stream_.get());
    QString save_error = file_signal_writer->save(*file_signal_reader_.get(), event_vector,
                                                  file_name, true);

    if (save_error.size() == 0)
    {
        //setState(STATE_FILE_OPENED);
    }
    else
    {
        *log_stream_ << "MainWindowModel::fileSaveAction Error: writing file :'"
                     << file_name << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName() + ": \"" + save_error + "\"");
    }

    delete file_signal_writer;
}

// file save as action

void MainWindowModel::fileSaveAsAction()
{
    if (!checkMainWindowPtr("fileSaveAsAction") ||
        !file_context_)
    {
        return;
    }

    // save as dialog
    QSettings settings("SigViewer");
    QString path = settings.value("MainWindowModel/file_open_path", ".").toString();
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
        *log_stream_ << "MainWindowModel::fileSaveAsAction Error: "
                     << "not suported file format: '"
                     << file_name.mid(file_name.lastIndexOf('.')) << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName());
        return;
    }

    // get events from buffer
    FileSignalReader::SignalEventVector event_vector;
    signal_browser_model_->getEvents(event_vector);

    // save
    file_signal_writer->setLogStream(log_stream_.get());

    QString save_error = file_signal_writer->save(*file_signal_reader_.get(), event_vector,
                                                  file_name, true);

    if (save_error.size () == 0)
    {
        file_signal_reader_->close();
        file_signal_reader_->open(file_name, overflow_detection_);
        //setState(STATE_FILE_OPENED);
    }
    else
    {
        *log_stream_ << "MainWindowModel::fileSaveAsAction "
                     << "Error: writing file:'" << file_name << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName() + ": \"" + tr(save_error.toLatin1()) + "\"");
    }

    delete file_signal_writer;
}

// file export events action
void MainWindowModel::fileExportEventsAction()
{
    if (!checkMainWindowPtr("fileExportEventsAction") ||
        !file_context_)
    {
        return;
    }

    // get events
    FileSignalReader::SignalEventVector event_vector;

    signal_browser_model_->getEvents(event_vector);

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
                                      *event_color_manager_.get(),
                                      *event_table_file_reader_.data(),
                                      main_window_);

    event_type_dialog.setShownTypes(event_types); //, signal_browser_model_->isShowAllEventTypes());
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

    file_name = settings.value("MainWindowModel/file_open_path", ".").toString() + "/" +
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
        *log_stream_ << "MainWindowModel::fileExportEventsAction Error: "
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

    QString save_error = file_signal_writer->save(*file_signal_reader_.get(), event_vector,
                                                  file_name, false);

    if (save_error.size () == 0)
    {
        //setState(STATE_FILE_OPENED);
    }
    else
    {
        *log_stream_ << "MainWindowModel::fileExportEventsAction "
                     << "Error: writing file:'" << file_name << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName() + ": \""+ tr(save_error.toLatin1()) + "\"");
    }

    delete file_signal_writer;
}

// file import events action
void MainWindowModel::fileImportEventsAction()
{
    if (!checkMainWindowPtr("fileImportEventsAction") ||
        !file_context_)
    {
        return;
    }

    // import dialog
    QSettings settings("SigViewer");
    QString path = settings.value("MainWindowModel/file_open_path", ".").toString();
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
        *log_stream_ << "MainWindowModel::fileImportEventsAction Error: "
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
                                      *event_color_manager_.get(),
                                      *event_table_file_reader_.data(),
                                      main_window_);

    event_type_dialog.setShownTypes(event_types); //, signal_browser_model_->isShowAllEventTypes());
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
            signal_browser_model_->addEvent(QSharedPointer<SignalEvent>(new SignalEvent(*it)), false);
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
void MainWindowModel::recentFileMenuAboutToShow()
{
    main_window_->setRecentFiles(recent_file_list_);
}

// recent file activated
void MainWindowModel::recentFileActivated(QAction* recent_file_action)
{
    if (!checkMainWindowPtr("recentFileActivated"))
    {
        return;
    }

    openFile(recent_file_action->text());
}

// file open
void MainWindowModel::openFile(const QString& file_name)
{
    // close
    if (file_context_)
    {
        fileCloseAction();

        if (application_context_.getState() != APP_STATE_NO_FILE_OPEN)
        {
            return; // user cancel
        }
    }

    file_context_ = new FileContext ();
    application_context_.getGUIActionManager().connect(file_context_, SIGNAL(stateChanged(FileState)), SLOT(setFileState(FileState)));

    // open siganl reader
    FileSignalReader* signal_reader = 0;
    QString load;

    if (file_name.lastIndexOf('.') != -1)
    {
        signal_reader = FileSignalReaderFactory::getInstance()->
                    getElement(file_name.mid(file_name.lastIndexOf('.')));

        if (signal_reader)
        {
            signal_reader->setLogStream(log_stream_.get());
            load = signal_reader->open(file_name, overflow_detection_);

            if (load.size ())
            {
                delete signal_reader;
            }
        }
    }

    if (load.size ())
    {
        *log_stream_ << "MainWindowModel::openFile Error: "
                     << "file format: '" << file_name
                     << "'\n";

        main_window_->showErrorReadDialog(file_name + " \"" + load + "\"");
        return;
    }

    // loading successfull
    file_signal_reader_.reset(signal_reader);
    int32 sep_pos = file_name.lastIndexOf(DIR_SEPARATOR);
    QString path = sep_pos == -1 ? "." : file_name.mid(0, sep_pos);
    QSettings settings("SigViewer");
    settings.setValue("MainWindowModel/file_open_path", path);


    // initialize signal browser
    TabContext* tab_context = new TabContext ();
    signal_browser_model_ = QSharedPointer<SignalBrowserModel>(new SignalBrowserModel(*signal_reader, *this, event_table_file_reader_, *tab_context));
    signal_browser_model_->setLogStream(log_stream_.get());

    if (!tab_widget_)
    {
        tab_widget_ = new QTabWidget (main_window_);
        connect (tab_widget_, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
        connect (tab_widget_, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
        tab_widget_->setTabsClosable (true);
    }

    signal_browser_ = new SignalBrowserView(signal_browser_model_, tab_widget_);
    signal_browser_model_->setSignalBrowserView(signal_browser_);
    signal_browser_model_->loadSettings();

    int tab_index = tab_widget_->addTab(signal_browser_, tr("Signal Data"));
    tab_contexts_[tab_index] = tab_context;

    application_context_.getGUIActionManager().connect(tab_context, SIGNAL(stateChanged(TabContext::State)), SLOT(setTabState(TabContext::State)));

    tab_context->setState (TabContext::NO_EVENT_SELECTED);
    browser_models_[tab_index] = signal_browser_model_;

    tab_widget_->hide();

    main_window_->setCentralWidget(tab_widget_);


    application_context_.setState(APP_STATE_FILE_OPEN);

    // update recent files
    recent_file_list_.removeAll(file_name);
    if (recent_file_list_.size() == number_recent_files_)
    {
        recent_file_list_.pop_back();
    }

    recent_file_list_.push_front(file_name);

    // select channels
    channelSelection ();

    tab_widget_->show();
    signal_browser_->show();

    // set signals per page to all
    int32 nr_shown_channels = signal_browser_model_->getNumberShownChannels();
    nr_shown_channels = nr_shown_channels == 0 ? 1 : nr_shown_channels;

    int32 signal_height = (int32)(signal_browser_->getVisibleHeight() /
                                  nr_shown_channels) -
                          (signal_browser_model_->getSignalSpacing() * 2);

    signal_browser_model_->setItemsHeight(signal_height);
    main_window_->setSignalsPerPage(-1); // all

    // set status bar
    main_window_->setStatusBarSignalLength(file_signal_reader_->getBasicHeader()->getNumberRecords() *
                                           file_signal_reader_->getBasicHeader()->getRecordDuration());

    main_window_->setStatusBarNrChannels(file_signal_reader_->getBasicHeader()->getNumberChannels());

    // show signal_browser
    signal_browser_model_->autoScaleAll(); // autoscale on startup


    secsPerPageChanged(secs_per_page_);
    main_window_->setSecsPerPage(secs_per_page_);
    signal_browser_model_->updateLayout();

}

// file close action
void MainWindowModel::fileCloseAction()
{
    if (!checkMainWindowPtr("fileCloseAction") ||
        !file_context_)
    {
        return;
    }

    if (file_context_->getState() == FILE_STATE_CHANGED &&
        !main_window_
        ->showFileCloseDialog(file_signal_reader_->getBasicHeader()->getFullFileName()))
    {
        return; // user cancel
    }

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
    file_signal_reader_.reset(0);


    // reset status bar
    main_window_->setStatusBarSignalLength(-1);
    main_window_->setStatusBarNrChannels(-1);

    delete file_context_;
    file_context_ = 0;

    application_context_.setState(APP_STATE_NO_FILE_OPEN);
}

// file info action
void MainWindowModel::fileInfoAction()
{
    if (!checkMainWindowPtr("fileInfoAction") ||
        !file_context_)
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
void MainWindowModel::fileExitAction()
{
    if (!checkMainWindowPtr("fileExitAction"))
    {
        return;
    }

    if (file_context_)
    {
        fileCloseAction();

        if (file_context_)
        {
            return; // user cancel;
        }
    }

    QApplication::exit();
}

// edit to all channels action
void MainWindowModel::editToAllChannelsAction()
{
    if (!checkMainWindowPtr("editToAllChannelsAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->setSelectedEventToAllChannels();
}

// edit copy to channles action
void MainWindowModel::editCopyToChannelsAction()
{
    if (!checkMainWindowPtr("editCopyToChannelsAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->copySelectedEventToChannels();
}

// edit delete action
void MainWindowModel::editDeleteAction()
{
    if (!checkMainWindowPtr("editDeleteAction") ||
        !file_context_)
    {
        return;
    }

    QUndoCommand* deleteCommand = new DeleteEventUndoCommand (*signal_browser_model_, signal_browser_model_->getSelectedEventItem());
    CommandStack::instance().executeEditCommand(deleteCommand);
}

// edit change channel action
void MainWindowModel::editChangeChannelAction()
{
    if (!checkMainWindowPtr("editChangeChannelAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->changeSelectedEventChannel();
}

// edit change type action
void MainWindowModel::editChangeTypeAction()
{
    if (!checkMainWindowPtr("editChangeTypeAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->changeSelectedEventType();
}

// edit event table action
void MainWindowModel::editEventTableAction()
{
    if (!checkMainWindowPtr("editEventTableAction") ||
        !file_context_)
    {
        return;
    }

    EventTableDialog event_table_dialog(signal_browser_model_,
                                        file_signal_reader_->getBasicHeader(),
                                        main_window_);
    event_table_dialog.loadSettings();
    event_table_dialog.exec();
    event_table_dialog.saveSettings();
}

// view zoom in action
void MainWindowModel::viewZoomInAction()
{
    if (!checkMainWindowPtr("viewZoomInAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->zoomInAll();
}

// mouse mode new action
void MainWindowModel::mouseModeNewAction()
{
    if (!checkMainWindowPtr("mouseModeNewAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->unsetSelectedEventItem();
    signal_browser_model_->setMode(MODE_NEW);
    signal_browser_->setScrollMode(false);
}

// mouse mode pointer action
void MainWindowModel::mouseModePointerAction()
{
    if (!checkMainWindowPtr("mouseModePointerAction") ||
        !file_context_)
    {
        return;
    }
    signal_browser_model_->setMode(MODE_POINTER);
    signal_browser_->setScrollMode(false);
}

// mouse mode hand action
void MainWindowModel::mouseModeHandAction()
{
    if (!checkMainWindowPtr("mouseModeHandAction") ||
        !file_context_)
    {
        return;
    }
    signal_browser_model_->setMode(MODE_HAND);
    signal_browser_->setScrollMode(true);
}

// mouse mode shift signal action
void MainWindowModel::mouseModeShiftSignalAction()
{
    if (!checkMainWindowPtr("mouseModeShiftSignalAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->setMode(MODE_SHIFT_SIGNAL);
    signal_browser_->setScrollMode(false);
}

// mouse mode zoom action
//void MainWindowModel::mouseModeZoomAction()
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
void MainWindowModel::viewZoomOutAction()
{
    if (!checkMainWindowPtr("viewZoomOutAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->zoomOutAll();
}

// view auto scale action
void MainWindowModel::viewAutoScaleAction()
{
    if (!checkMainWindowPtr("viewAutoScaleAction") ||
        !file_context_)
    {
        return;
    }

    signal_browser_model_->autoScaleAll();
}

// view go to action
void MainWindowModel::viewGoToAction()
{
    if (!checkMainWindowPtr("viewGoToAction") ||
        !file_context_)
    {
        return;
    }

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
void MainWindowModel::viewShowAndSelectNextEventAction()
{
    if (!checkMainWindowPtr("viewSelectNextEventAction") ||
        !file_context_)
    {
        return;
    }

    QUndoCommand* eventCommand = new NextEventViewUndoCommand (*signal_browser_model_);
    CommandStack::instance().executeViewCommand(eventCommand);
}

//-------------------------------------------------------------------
// view select previous event action
void MainWindowModel::viewShowAndSelectPreviousEventAction()
{
    if (!checkMainWindowPtr("viewShowAndSelectPreviousEventAction") ||
        !file_context_)
    {
        return;
    }

    QUndoCommand* eventCommand = new NextEventViewUndoCommand (*signal_browser_model_, false);
    CommandStack::instance().executeViewCommand(eventCommand);
}

//-------------------------------------------------------------------
// view select next event action
void MainWindowModel::viewShowEventsOfSelectedTypeAction()
{
    if (!checkMainWindowPtr("viewShowEventsOfSelectedTypeAction") ||
        !file_context_)
    {
        return;
    }

    SignalBrowserModel::IntList shown_event_types;
    shown_event_types.append (signal_browser_model_->getSelectedSignalEvent()->getType());

    QUndoCommand* eventCommand = new SetShownEventTypesViewUndoCommand (*signal_browser_model_, shown_event_types);
    CommandStack::instance().executeViewCommand(eventCommand);
}



//-------------------------------------------------------------------
// fits the view to the selected event action
void MainWindowModel::viewFitToEventAction()
{
    if (!checkMainWindowPtr("viewFitToEventAction") ||
        !file_context_)
    {
        return;
    }

    QUndoCommand* eventCommand = new FitViewToEventViewUndoCommand (*signal_browser_model_);
    CommandStack::instance().executeViewCommand(eventCommand);
}

// options channels action
void MainWindowModel::optionsChannelsAction()
{
    channelSelection ();

    // mouse mode

    //main_window_->setMouseMode(signal_browser_model_->getMode());



    // set signals per page to all

    //int32 nr_shown_channels = signal_browser_model_->getNumberShownChannels();

    //nr_shown_channels = nr_shown_channels == 0 ? 1 : nr_shown_channels;

    //int32 signal_height = (int32)(signal_browser_ ->getCanvasView()->visibleHeight() /

    //                              nr_shown_channels) -

    //                      signal_browser_model_->getSignalSpacing();

    //signal_browser_model_->setSignalHeight(signal_height);

    //main_window_->setSignalsPerPage(-1); // all



    // set secs per page 10

    //signal_browser_model_->setPixelPerSec(signal_browser_->getCanvasView()->visibleWidth() / 10.0);

    //main_window_->setSecsPerPage(10.0);



    // set status bar

    //main_window_->setStatusBarSignalLength(file_signal_reader_->getBasicHeader()->getNumberRecords() *

    //                                       file_signal_reader_->getBasicHeader()->getRecordDuration());

    //main_window_->setStatusBarNrChannels(file_signal_reader_->getBasicHeader()->getNumberChannels());



    // show signal_browser

    //signal_browser_model_->autoScaleAll(); // autoscal on startup
    signal_browser_->show();

}

void MainWindowModel::channelSelection ()
{
    if (!checkMainWindowPtr("optionsChannelsAction") ||
        !file_context_)
    {
        return;
    }

    ChannelSelectionDialog channel_dialog(file_signal_reader_->getBasicHeader(),
                                          main_window_);

    // current selected channels

    bool empty_selection = signal_browser_model_->getNumberShownChannels() == 0;

    for (uint32 channel_nr = 0;
         channel_nr < file_signal_reader_->getBasicHeader()->getNumberChannels();
         channel_nr++)
    {
        bool show_channel = empty_selection ||
                            signal_browser_model_->isChannelShown(channel_nr);

        channel_dialog.setSelected(channel_nr, show_channel);
    }

    channel_dialog.loadSettings();
    channel_dialog.exec();
    channel_dialog.saveSettings();

    if (channel_dialog.result() == QDialog::Rejected)
    {
        return; // user cancel
    }

    //signal_browser_model_->enableInitMinMaxSearch(channel_dialog.isInitRangeSearch());
    signal_browser_model_->enableInitMinMaxSearch (true);

    for (uint32 channel_nr = 0;
         channel_nr < file_signal_reader_->getBasicHeader()->getNumberChannels();
         channel_nr++)
    {
        if (channel_dialog.isSelected(channel_nr))
        {
            signal_browser_model_->addChannel(channel_nr);
        }
        else
        {
            signal_browser_model_->removeChannel(channel_nr);
        }
    }
    signal_browser_model_->initBuffer();
    signal_browser_model_->updateLayout();
}

// options show events action
void MainWindowModel::optionsShowEventsAction()
{
    if (!checkMainWindowPtr("optionsShowEventsAction") ||
        !file_context_)
    {
        return;
    }

    // set curent shown event types
    EventTypeDialog::IntList shown_event_types;

    signal_browser_model_->getShownEventTypes(shown_event_types);

    EventTypeDialog event_type_dialog(tr("Show Events"),
                                      *event_color_manager_.get(),
                                      *event_table_file_reader_.data(),
                                      main_window_);

    event_type_dialog.setShownTypes(shown_event_types, signal_browser_model_->isShowAllEventTypes());
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
void MainWindowModel::optionsShowSettingsAction()
{
    if (!file_context_)
    {
        return;
    }
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

    if (file_context_) {
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
}

// help log action
void MainWindowModel::helpLogAction()
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
void MainWindowModel::helpAboutAction()
{
    if (!checkMainWindowPtr("helpAboutAction"))
    {
        return;
    }

    main_window_->showHelpAboutDialog();
}

// check main window ptr
inline bool MainWindowModel::checkMainWindowPtr(const QString function)
{
    if (!main_window_)
    {
        *log_stream_ << "MainWindowModel::" << function
                     << " Error: MainWindow not set\n";
        return false;
    }

    return true;
}

// secs per page changed
void MainWindowModel::secsPerPageChanged(const QString& secs_per_page)
{
    if (!checkMainWindowPtr("secsPerPageChanged") ||
        !file_context_)
    {
        return;
    }

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
void MainWindowModel::signalsPerPageChanged(const QString& signals_per_page)
{
    if (!checkMainWindowPtr("signalsPerPageChanged") ||
        !file_context_)
    {
        return;
    }

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
void MainWindowModel::pixelPerSecChanged(float64 pixel_per_sec)
{
    if (!checkMainWindowPtr("pixelPerSecChanged") ||
        !file_context_)
    {
        return;
    }

    float64 secs_per_page = signal_browser_->getVisibleWidth() /
                            pixel_per_sec;

    secs_per_page = (int32)(secs_per_page * 10) / 10.0;
    main_window_->setSecsPerPage(secs_per_page);
}

// signal height changed
void MainWindowModel::signalHeightChanged(int32 signal_height)
{
    if (!checkMainWindowPtr("signalHeightChanged") ||
        !file_context_)
    {
        return;
    }

    float64 signals_per_page;
    signals_per_page = signal_browser_->getVisibleHeight() /
                       (float64)(signal_height +
                                 signal_browser_model_->getSignalSpacing());

    signals_per_page = (int32)(signals_per_page * 10) / 10.0;
    main_window_->setSignalsPerPage(signals_per_page);
}

// set changed
void MainWindowModel::setChanged()
{
    if (!checkMainWindowPtr("setChanged"))
    {
        return;
    }

    if (!file_context_)
    {
        *log_stream_ << "MainWindowModel::setChanged"
                     << " Error: illegal state\n";
        return;
    }
    else
        file_context_->setState (FILE_STATE_CHANGED);
}

//-----------------------------------------------------------------------------
QSharedPointer<BlocksVisualisationModel> MainWindowModel::createBlocksVisualisationView (QString const& title)
{
    BlocksVisualisationView* bv_view = new BlocksVisualisationView (tab_widget_);
    SignalBuffer const& signal_buffer = signal_browser_model_->getSignalBuffer();
    QSharedPointer<BlocksVisualisationModel> bv_model = QSharedPointer<BlocksVisualisationModel> (new BlocksVisualisationModel (bv_view, signal_browser_model_->getPixelPerXUnit (), static_cast<float64>(signal_buffer.getRecordsPerBlock()) / signal_buffer.getBlockDuration()));

    blocks_visualisation_models_.push_back (bv_model);
    int tab_index = tab_widget_->addTab(bv_view, title);
    browser_models_[tab_index] = bv_model;
    tab_widget_->setCurrentWidget(bv_view);

    return bv_model;
}


} // namespace BioSig_

