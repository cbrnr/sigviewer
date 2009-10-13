// main_window_model.cpp

#include "main_window_model.h"
#include "main_window.h"
#include "base/file_signal_reader_factory.h"
#include "base/file_signal_writer_factory.h"
#include "base/event_table_file_reader.h"
#include "basic_header_info_dialog.h"
#include "log_dialog.h"
#include "channel_selection_dialog.h"
#include "signal_browser.h"
#include "signal_browser_model.h"
#include "signal_browser.h"
#include "go_to_dialog.h"
#include "smart_canvas/smart_canvas.h"
#include "smart_canvas/smart_canvas_view.h"
#include "event_type_dialog.h"
#include "gui_signal_buffer.h"
#include "event_table_dialog.h"
#include "event_color_manager.h"
#include "settings_dialog.h"
#include "command_stack.h"

// QT4
#include "signal_browser/signal_browser_model_4.h"
#include "signal_browser/signal_browser_view.h"
#include "signal_browser/delete_event_undo_command.h"
#include "next_event_view_undo_command.h"

#include <QString>
#include <QApplication>
#include <QFile>
#include <QAction>
#include <QTextStream>
#include <QSettings>
#include <iostream>

namespace BioSig_
{

// constructor
MainWindowModel::MainWindowModel()
: main_window_(0),
  state_(STATE_FILE_CLOSED),
  selection_state_(SELECTION_STATE_NONE),
  number_recent_files_(8),
  secs_per_page_("10"),
  overflow_detection_(false)
{
    log_stream_.reset(new QTextStream(&log_string_));
    file_signal_reader_.reset(0);

    signal_browser_model_.reset(0);
    signal_browser_.reset(0);

    event_table_file_reader_.reset(new EventTableFileReader);
    event_table_file_reader_->setLogStream(log_stream_.get());
    event_color_manager_
        .reset(new EventColorManager(*event_table_file_reader_.get()));
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
EventTableFileReader& MainWindowModel::getEventTableFileReader()
{
    return *event_table_file_reader_.get();
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
    setState(STATE_FILE_CLOSED);
}

// get main window
MainWindow* MainWindowModel::getMainWindow()
{
    return main_window_;
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

// get state
MainWindowModel::State MainWindowModel::getState()
{
    return state_;
}

// set state
void MainWindowModel::setState(MainWindowModel::State state)
{
    state_ = state;

    switch(state_)
    {
        case STATE_EXIT:
            QApplication::exit();
            // main_window->close();
            break;

        case STATE_FILE_CLOSED:
            if (main_window_)
            {
                main_window_->setCaption("SigViewer");
                main_window_->setFileSaveEnabled(false);
                main_window_->setFileSaveAsEnabled(false);
                main_window_->SetFileExportEventsEnabled(false);
                main_window_->SetFileImportEventsEnabled(false);
                main_window_->setFileCloseEnabled(false);
                main_window_->setFileInfoEnabled(false);
                main_window_->setEditEventTableEnabled(false);
                main_window_->setEditChangeTypeEnabled(false);
                main_window_->setMouseModeNewEnabled(false);
                main_window_->setMouseModePointerEnabled(false);
                main_window_->setMouseModeHandEnabled(false);
                main_window_->setMouseModeShiftSignalEnabled(false);
                main_window_->setMouseModeZoomEnabled(false);
                main_window_->setOptionsChannelsEnabled(false);
                main_window_->setOptionsShowEventsEnabled(false);
                main_window_->setViewZoomInEnabled(false);
                main_window_->setViewZoomOutEnabled(false);
                main_window_->setViewAutoScaleEnabled(false);
                main_window_->setViewGoToEnabled(false);
                main_window_->setSecsPerPageEnabled(false);
                main_window_->setSignalsPerPageEnabled(false);

                setSelectionState(SELECTION_STATE_OFF);
            }

            break;

        case STATE_FILE_OPENED:
            if (main_window_)
            {
                QString caption = "SigViewer - [%1]";
                caption = caption.arg(file_signal_reader_->getBasicHeader()->getFullFileName());
                main_window_->setCaption(caption);
                main_window_->setFileSaveEnabled(false);
                main_window_->setFileSaveAsEnabled(true);
                main_window_->SetFileExportEventsEnabled(true);
                main_window_->SetFileImportEventsEnabled(true);
                main_window_->setFileCloseEnabled(true);
                main_window_->setFileInfoEnabled(true);
                main_window_->setEditEventTableEnabled(true);
                main_window_->setEditChangeTypeEnabled(true);
                main_window_->setMouseModeNewEnabled(true);
                main_window_->setMouseModePointerEnabled(true);
                main_window_->setMouseModeHandEnabled(true);
                main_window_->setMouseModeShiftSignalEnabled(true);
                main_window_->setMouseModeZoomEnabled(true);
                main_window_->setOptionsChannelsEnabled(true);
                main_window_->setOptionsShowEventsEnabled(true);
                main_window_->setViewZoomInEnabled(true);
                main_window_->setViewZoomOutEnabled(true);
                main_window_->setViewAutoScaleEnabled(true);
                main_window_->setViewGoToEnabled(true);
                main_window_->setSecsPerPageEnabled(true);
                main_window_->setSignalsPerPageEnabled(true);
            }

            break;

        case STATE_FILE_CHANGED:
            if (main_window_)
            {
                QString caption = "SigViewer - [%1*]";
                caption = caption.arg(file_signal_reader_->getBasicHeader()->getFullFileName());
                main_window_->setCaption(caption);
                main_window_->setFileSaveEnabled(true);
                main_window_->setFileSaveAsEnabled(true);
                main_window_->SetFileExportEventsEnabled(true);
                main_window_->SetFileImportEventsEnabled(true);
                main_window_->setFileCloseEnabled(true);
                main_window_->setFileInfoEnabled(true);
                main_window_->setEditEventTableEnabled(true);
                main_window_->setEditChangeTypeEnabled(true);
                main_window_->setMouseModeNewEnabled(true);
                main_window_->setMouseModePointerEnabled(true);
                main_window_->setMouseModeHandEnabled(true);
                main_window_->setMouseModeShiftSignalEnabled(true);
                main_window_->setMouseModeZoomEnabled(true);
                main_window_->setOptionsChannelsEnabled(true);
                main_window_->setOptionsShowEventsEnabled(true);
                main_window_->setViewZoomInEnabled(true);
                main_window_->setViewZoomOutEnabled(true);
                main_window_->setViewAutoScaleEnabled(true);
                main_window_->setViewGoToEnabled(true);
                main_window_->setSecsPerPageEnabled(true);
                main_window_->setSignalsPerPageEnabled(true);
            }

            break;
    }
}

// set selection state
void MainWindowModel::setSelectionState(SelectionState selection_state)
{
    selection_state_ = selection_state;

    switch(selection_state_)
    {
        case SELECTION_STATE_OFF:
            main_window_->setEditToAllChannelsEnabled(false);
            main_window_->setEditCopyToChannelsEnabled(false);
            main_window_->setEditDeleteEnabled(false);
            main_window_->setEditChangeChannelEnabled(false);
            main_window_->setEditChangeTypeEnabled(false);

            break;

        case SELECTION_STATE_NONE:
            main_window_->setEditToAllChannelsEnabled(false);
            main_window_->setEditCopyToChannelsEnabled(false);
            main_window_->setEditDeleteEnabled(false);
            main_window_->setEditChangeChannelEnabled(false);
            main_window_->setEditChangeTypeEnabled(true);

            break;

        case SELECTION_STATE_ONE_CHANNEL:
            main_window_->setEditToAllChannelsEnabled(true);
            main_window_->setEditCopyToChannelsEnabled(true);
            main_window_->setEditDeleteEnabled(true);
            main_window_->setEditChangeChannelEnabled(true);
            main_window_->setEditChangeTypeEnabled(true);

            break;

        case SELECTION_STATE_ALL_CHANNELS:
            main_window_->setEditToAllChannelsEnabled(false);
            main_window_->setEditCopyToChannelsEnabled(false);
            main_window_->setEditDeleteEnabled(true);
            main_window_->setEditChangeChannelEnabled(true);
            main_window_->setEditChangeTypeEnabled(true);

            break;
    }

}

//-----------------------------------------------------------------------------
void MainWindowModel::undoAction()
{
    CommandStack::instance().undoLastEditCommand();
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
        !checkNotClosedState("fileCloseAction"))
    {
        return;
    }

    // get writer for file
    FileSignalWriter* file_signal_writer = 0;

    QString file_name = file_signal_reader_->getBasicHeader()->getFullFileName();

    if (file_name.findRev('.') != -1)
    {
        file_signal_writer = FileSignalWriterFactory::getInstance()->
                    getElement(file_name.mid(file_name.findRev('.')));
    }

    if (!file_signal_writer)
    {
        *log_stream_ << "MainWindowModel::fileSaveAction Error: "
                     << "not suported file format: '"
                     << file_name.mid(file_name.findRev('.')) << "'\n";

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
        setState(STATE_FILE_OPENED);
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
        !checkNotClosedState("fileSaveAsAction"))
    {
        return;
    }

    // save as dialog
    QSettings settings("SigViewer");
    QString path = settings.value("MainWindowModel/file_open_path", ".").toString();
    QString extensions = FileSignalWriterFactory::getInstance()->getExtensions();
    QString file_name;

    file_name = main_window_->showSaveAsDialog(path, extensions);

    if (file_name.isEmpty() || QFile::exists(file_name) &&
        !main_window_->showOverwriteDialog(file_name))
    {
        return; // user cancel
    }

    // get writer for file
    FileSignalWriter* file_signal_writer = 0;

    if (file_name.findRev('.') != -1)
    {
        file_signal_writer = FileSignalWriterFactory::getInstance()
                ->getElement(file_name.mid(file_name.findRev('.')));
    }

    if (!file_signal_writer)
    {
        *log_stream_ << "MainWindowModel::fileSaveAsAction Error: "
                     << "not suported file format: '"
                     << file_name.mid(file_name.findRev('.')) << "'\n";

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
        setState(STATE_FILE_OPENED);
    }
    else
    {
        *log_stream_ << "MainWindowModel::fileSaveAsAction "
                     << "Error: writing file:'" << file_name << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName() + ": \"" + tr(save_error) + "\"");
    }

    delete file_signal_writer;
}

// file export events action
void MainWindowModel::fileExportEventsAction()
{
    if (!checkMainWindowPtr("fileExportEventsAction") ||
        !checkNotClosedState("fileExportEventsAction"))
    {
        return;
    }

    // get events
    FileSignalReader::SignalEventVector event_vector;

    signal_browser_model_->getEvents(event_vector);

    // get event types
#ifndef QT4_PORTED
    SignalBrowserModel::IntList event_types;
#else
    PortingToQT4_::SignalBrowserModel::IntList event_types;
#endif
    FileSignalReader::SignalEventVector::iterator it;

    for (it = event_vector.begin(); it != event_vector.end(); it++)
    {
        event_types << it->getType();
    }

    qSort(event_types);

    // event type dialog
    EventTypeDialog event_type_dialog(tr("Export Events"),
                                      *event_color_manager_.get(),
                                      *event_table_file_reader_.get(),
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

    if (file_name.isEmpty() || QFile::exists(file_name) &&
        !main_window_->showOverwriteDialog(file_name))
    {
        return; // user cancel
    }

    // get writer for file
    FileSignalWriter* file_signal_writer = 0;

    if (file_name.findRev('.') != -1)
    {
        file_signal_writer = FileSignalWriterFactory::getInstance()
                    ->getElement(file_name.mid(file_name.findRev('.')));
    }

    if (!file_signal_writer)
    {
        *log_stream_ << "MainWindowModel::fileExportEventsAction Error: "
                     << "not suported file format: '"
                     << file_name.mid(file_name.findRev('.')) << "'\n";

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
        setState(STATE_FILE_OPENED);
    }
    else
    {
        *log_stream_ << "MainWindowModel::fileExportEventsAction "
                     << "Error: writing file:'" << file_name << "'\n";

        main_window_->showErrorWriteDialog(
                                file_signal_reader_->getBasicHeader()->getFullFileName() + ": \""+ tr(save_error) + "\"");
    }

    delete file_signal_writer;
}

// file import events action
void MainWindowModel::fileImportEventsAction()
{
    if (!checkMainWindowPtr("fileImportEventsAction") ||
        !checkNotClosedState("fileImportEventsAction"))
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
    if (file_name.findRev('.') != -1)
    {
        signal_reader = FileSignalReaderFactory::getInstance()
                    ->getElement(file_name.mid(file_name.findRev('.')));

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
                                      *event_table_file_reader_.get(),
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
    if (state_ == STATE_FILE_CHANGED || state_ == STATE_FILE_OPENED)
    {
        fileCloseAction();

        if (state_ != STATE_FILE_CLOSED)
        {
            return; // user cancel
        }
    }

    // open siganl reader
    FileSignalReader* signal_reader = 0;
    QString load;

    if (file_name.findRev('.') != -1)
    {
        signal_reader = FileSignalReaderFactory::getInstance()->
                    getElement(file_name.mid(file_name.findRev('.')));

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
    int32 sep_pos = file_name.findRev(DIR_SEPARATOR);
    QString path = sep_pos == -1 ? "." : file_name.mid(0, sep_pos);
    QSettings settings("SigViewer");
    settings.setValue("MainWindowModel/file_open_path", path);

    // initialize signal browser
#ifndef QT4_PORTED
    signal_browser_model_.reset(new SignalBrowserModel(*signal_reader, *this));
    signal_browser_model_->setLogStream(log_stream_.get());
    signal_browser_.reset(new SignalBrowser(*signal_browser_model_.get(),
                          main_window_));

    signal_browser_model_->setSignalBrowser(signal_browser_.get());
    signal_browser_model_->loadSettings();
    main_window_->setCentralWidget(signal_browser_.get());
#else
    signal_browser_model_.reset(new PortingToQT4_::SignalBrowserModel(*signal_reader, *this));
    signal_browser_model_->setLogStream(log_stream_.get());
    signal_browser_.reset (new PortingToQT4_::SignalBrowserView(signal_browser_model_.get(), main_window_));

    signal_browser_model_->setSignalBrowserView(signal_browser_.get());
    signal_browser_model_->loadSettings();
    main_window_->setCentralWidget(signal_browser_.get());

#endif // QT4_PORTED
    setState(STATE_FILE_OPENED);

    // update recent files
    recent_file_list_.removeAll(file_name);
    if (recent_file_list_.size() == number_recent_files_)
    {
        recent_file_list_.pop_back();
    }

    recent_file_list_.push_front(file_name);

    // select channels
    channelSelection ();

    // mouse mode
    main_window_->setMouseMode(signal_browser_model_->getMode());


    // set signals per page to all
#ifndef QT4_PORTED
    int32 nr_shown_channels = signal_browser_model_->getNumberShownChannels();
    nr_shown_channels = nr_shown_channels == 0 ? 1 : nr_shown_channels;
    int32 signal_height = (int32)(signal_browser_ ->getCanvasView()->visibleHeight() /
                                  nr_shown_channels) -
                          signal_browser_model_->getSignalSpacing();

    signal_browser_model_->setSignalHeight(signal_height);
#else
    int32 nr_shown_channels = signal_browser_model_->getNumberShownChannels();
    nr_shown_channels = nr_shown_channels == 0 ? 1 : nr_shown_channels;

    int32 signal_height = (int32)(signal_browser_->getVisibleHeight() /
                                  nr_shown_channels) -
                          (signal_browser_model_->getSignalSpacing() * 2);

    signal_browser_model_->setSignalHeight(signal_height);
#endif // QT4_PORTED
    main_window_->setSignalsPerPage(-1); // all

    // set status bar
    main_window_->setStatusBarSignalLength(file_signal_reader_->getBasicHeader()->getNumberRecords() *
                                           file_signal_reader_->getBasicHeader()->getRecordDuration());

    main_window_->setStatusBarNrChannels(file_signal_reader_->getBasicHeader()->getNumberChannels());

    // show signal_browser
    signal_browser_model_->autoScaleAll(); // autoscale on startup

    signal_browser_->show();

    secsPerPageChanged(secs_per_page_);
    main_window_->setSecsPerPage(secs_per_page_);
}

// file close action
void MainWindowModel::fileCloseAction()
{
    if (!checkMainWindowPtr("fileCloseAction") ||
        !checkNotClosedState("fileCloseAction"))
    {
        return;
    }

    if (state_ == STATE_FILE_CHANGED &&
        !main_window_
        ->showFileCloseDialog(file_signal_reader_->getBasicHeader()->getFullFileName()))
    {
        return; // user cancel
    }

    // close
    signal_browser_model_->saveSettings();
    file_signal_reader_->close();
    file_signal_reader_.reset(0);
    signal_browser_model_.reset(0);
    signal_browser_.reset(0);
    main_window_->setCentralWidget(0);

    // reset status bar
    main_window_->setStatusBarSignalLength(-1);
    main_window_->setStatusBarNrChannels(-1);

    setState(STATE_FILE_CLOSED);
}

// file info action
void MainWindowModel::fileInfoAction()
{
    if (!checkMainWindowPtr("fileInfoAction") ||
        !checkNotClosedState("fileInfoAction"))
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

    if (state_ == STATE_FILE_CHANGED || state_ == STATE_FILE_OPENED)
    {
        fileCloseAction();

        if (state_ != STATE_FILE_CLOSED)
        {
            return; // user cancel;
        }
    }

    setState(STATE_EXIT);
}

// edit to all channels action
void MainWindowModel::editToAllChannelsAction()
{
    if (!checkMainWindowPtr("editToAllChannelsAction") ||
        !checkNotClosedState("editToAllChannelsAction"))
    {
        return;
    }

    signal_browser_model_->setSelectedEventToAllChannels();
}

// edit copy to channles action
void MainWindowModel::editCopyToChannelsAction()
{
    if (!checkMainWindowPtr("editCopyToChannelsAction") ||
        !checkNotClosedState("editCopyToChannelsAction"))
    {
        return;
    }

    signal_browser_model_->copySelectedEventToChannels();
}

// edit delete action
void MainWindowModel::editDeleteAction()
{
    if (!checkMainWindowPtr("editDeleteAction") ||
        !checkNotClosedState("editDeleteAction"))
    {
        return;
    }

#ifndef QT4_PORTED
    signal_browser_model_->removeSelectedEvent();
#else
    QUndoCommand* deleteCommand = new PortingToQT4_::DeleteEventUndoCommand (*signal_browser_model_, signal_browser_model_->getSelectedEventItem());
    CommandStack::instance().executeEditCommand(deleteCommand);
#endif
}

// edit change channel action
void MainWindowModel::editChangeChannelAction()
{
    if (!checkMainWindowPtr("editChangeChannelAction") ||
        !checkNotClosedState("editChangeChannelAction"))
    {
        return;
    }

    signal_browser_model_->changeSelectedEventChannel();
}

// edit change type action
void MainWindowModel::editChangeTypeAction()
{
    if (!checkMainWindowPtr("editChangeTypeAction") ||
        !checkNotClosedState("editChangeTypeAction"))
    {
        return;
    }

    signal_browser_model_->changeSelectedEventType();
}

// edit event table action
void MainWindowModel::editEventTableAction()
{
    if (!checkMainWindowPtr("editEventTableAction") ||
        !checkNotClosedState("editEventTableAction"))
    {
        return;
    }

#ifndef QT4_PORTED
    EventTableDialog event_table_dialog(*signal_browser_model_.get(),
                                        file_signal_reader_->getBasicHeader(),
                                        main_window_);
#else
    EventTableDialog event_table_dialog(*signal_browser_model_.get(),
                                        file_signal_reader_->getBasicHeader(),
                                        main_window_);
#endif

    event_table_dialog.loadSettings();
    event_table_dialog.exec();
    event_table_dialog.saveSettings();
}

// view zoom in action
void MainWindowModel::viewZoomInAction()
{
    if (!checkMainWindowPtr("viewZoomInAction") ||
        !checkNotClosedState("viewZoomInAction"))
    {
        return;
    }

    signal_browser_model_->zoomInAll();
}

// mouse mode new action
void MainWindowModel::mouseModeNewAction()
{
    if (!checkMainWindowPtr("mouseModeNewAction") ||
        !checkNotClosedState("mouseModeNewAction"))
    {
        return;
    }

#ifndef QT4_PORTED
    signal_browser_model_->setSelectedEventItem(0);
    signal_browser_model_->setMode(SignalBrowserModel::MODE_NEW);
#else
    signal_browser_model_->unsetSelectedEventItem();
    signal_browser_model_->setMode(PortingToQT4_::SignalBrowserModel::MODE_NEW);
    signal_browser_->setScrollMode(false);
#endif
}

// mouse mode pointer action
void MainWindowModel::mouseModePointerAction()
{
    if (!checkMainWindowPtr("mouseModePointerAction") ||
        !checkNotClosedState("mouseModePointerAction"))
    {
        return;
    }
#ifndef QT4_PORTED
    signal_browser_model_->setMode(SignalBrowserModel::MODE_POINTER);
#else
    signal_browser_model_->setMode(PortingToQT4_::SignalBrowserModel::MODE_POINTER);
    signal_browser_->setScrollMode(false);
#endif
}

// mouse mode hand action
void MainWindowModel::mouseModeHandAction()
{
    if (!checkMainWindowPtr("mouseModeHandAction") ||
        !checkNotClosedState("mouseModeHandAction"))
    {
        return;
    }
#ifndef QT4_PORTED
    signal_browser_model_->setMode(SignalBrowserModel::MODE_HAND);
#else
    signal_browser_model_->setMode(PortingToQT4_::SignalBrowserModel::MODE_HAND);
    signal_browser_->setScrollMode(true);
#endif
}

// mouse mode shift signal action
void MainWindowModel::mouseModeShiftSignalAction()
{
    if (!checkMainWindowPtr("mouseModeShiftSignalAction") ||
        !checkNotClosedState("mouseModeShiftSignalAction"))
    {
        return;
    }

#ifndef QT4_PORTED
    signal_browser_model_->setMode(SignalBrowserModel::MODE_SHIFT_SIGNAL);
#else
    signal_browser_model_->setMode(PortingToQT4_::SignalBrowserModel::MODE_SHIFT_SIGNAL);
    signal_browser_->setScrollMode(false);
#endif
}

// mouse mode zoom action
void MainWindowModel::mouseModeZoomAction()
{
    if (!checkMainWindowPtr("mouseModeZoomAction") ||
        !checkNotClosedState("mouseModeZoomAction"))
    {
        return;
    }

#ifndef QT4_PORTED
    signal_browser_model_->setMode(SignalBrowserModel::MODE_ZOOM);
#else
    signal_browser_model_->setMode(PortingToQT4_::SignalBrowserModel::MODE_ZOOM);
    signal_browser_->setScrollMode(false);
#endif
}

// view zoom out action
void MainWindowModel::viewZoomOutAction()
{
    if (!checkMainWindowPtr("viewZoomOutAction") ||
        !checkNotClosedState("viewZoomOutAction"))
    {
        return;
    }

    signal_browser_model_->zoomOutAll();
}

// view auto scale action
void MainWindowModel::viewAutoScaleAction()
{
    if (!checkMainWindowPtr("viewAutoScaleAction") ||
        !checkNotClosedState("viewAutoScaleAction"))
    {
        return;
    }

    signal_browser_model_->autoScaleAll();
}

// view go to action
void MainWindowModel::viewGoToAction()
{
    if (!checkMainWindowPtr("viewGoToAction") ||
        !checkNotClosedState("viewGoToAction"))
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
        !checkNotClosedState("viewSelectNextEventAction"))
    {
        return;
    }

#ifndef QT4_PORTED
    // not possible in this version
#else
    QUndoCommand* eventCommand = new PortingToQT4_::NextEventViewUndoCommand (*signal_browser_model_);
    CommandStack::instance().executeViewCommand(eventCommand);
#endif
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
        !checkNotClosedState("optionsChannelsAction"))
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


#ifndef QT4_PORTED
    signal_browser_model_->enableInitDownsampling(channel_dialog.
                                                     isInitRangeSearch());
    signal_browser_model_->enableInitMinMaxSearch(channel_dialog.
                                                     isInitRangeSearch());
    signal_browser_model_->setWholeDataBuffer(
            (SignalBuffer::WHOLE_BUFFER)channel_dialog.wholeSubsampling());
    signal_browser_model_->setDefaultRange(channel_dialog.rangeMin(),
                                           channel_dialog.rangeMax());

    // set new selected channels
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

    // init buffer
    signal_browser_model_->initBuffer();

    // update layout
    signal_browser_model_->updateLayout();
#else
    // TODO: implement!
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
#endif // QT4_PORTED
}

// options show events action
void MainWindowModel::optionsShowEventsAction()
{
    if (!checkMainWindowPtr("optionsShowEventsAction") ||
        !checkNotClosedState("optionsShowEventsAction"))
    {
        return;
    }

    // set curent shown event types
    EventTypeDialog::IntList shown_event_types;

    signal_browser_model_->getShownEventTypes(shown_event_types);

    EventTypeDialog event_type_dialog(tr("Show Events"),
                                      *event_color_manager_.get(),
                                      *event_table_file_reader_.get(),
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
    signal_browser_model_->setShownEventTypes(shown_event_types, event_type_dialog.isAllSelected());

    // update layout
    signal_browser_model_->updateLayout();
}

// options show events action
void MainWindowModel::optionsShowSettingsAction()
{
    SettingsDialog settings_dialog(main_window_);

    settings_dialog.loadSettings();
    settings_dialog.exec();

    if (settings_dialog.result() == QDialog::Rejected)
    {
        return; // user cancel
    }

    // user ok
    settings_dialog.saveSettings();

    overflow_detection_ = settings_dialog.isOverflowDetection();

    if (state_ == STATE_FILE_OPENED) {
        signal_browser_model_->showChannelLabels(settings_dialog.isShowChannelLables());
        signal_browser_model_->showXScales(settings_dialog.isShowChannelScales());
        signal_browser_model_->showYScales(settings_dialog.isShowChannelScales());
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

// check not closed state
bool MainWindowModel::checkNotClosedState(const QString function)
{
    if (state_ == STATE_FILE_CLOSED)
    {
        *log_stream_ << "MainWindowModel::" << function
                     << " Error: illegal state\n";
        return false;
    }
    return true;
}

// secs per page changed
void MainWindowModel::secsPerPageChanged(const QString& secs_per_page)
{
    if (!checkMainWindowPtr("secsPerPageChanged") ||
        !checkNotClosedState("secsPerPageChanged"))
    {
        return;
    }

    bool ok = false;
    float64 f_secs_per_page = secs_per_page.toFloat(&ok);
    float64 pixel_per_sec;

    if (ok)
    {
#ifndef QT4_PORTED
        f_secs_per_page = (int32)(f_secs_per_page * 10) / 10.0;
        pixel_per_sec = signal_browser_->getCanvasView()->visibleWidth() /
                        f_secs_per_page;
#else
        f_secs_per_page = (int32)(f_secs_per_page * 10) / 10.0;
        pixel_per_sec = signal_browser_->getVisibleWidth() /
                        f_secs_per_page;
#endif // QT4_PORTED
    }
    else
    {
        // whole
#ifndef QT4_PORTED
        pixel_per_sec = signal_browser_->getCanvasView()->visibleWidth() /
                        (file_signal_reader_->getBasicHeader()->getNumberRecords() *
                         file_signal_reader_->getBasicHeader()->getRecordDuration());
#else
        pixel_per_sec = signal_browser_->getVisibleWidth() /
                        (file_signal_reader_->getBasicHeader()->getNumberRecords() *
                         file_signal_reader_->getBasicHeader()->getRecordDuration());
#endif // QT4_PORTED
    }
    signal_browser_model_->setPixelPerSec(pixel_per_sec);

    secs_per_page_ = secs_per_page;
}

// signals per page changed
void MainWindowModel::signalsPerPageChanged(const QString& signals_per_page)
{
    if (!checkMainWindowPtr("signalsPerPageChanged") ||
        !checkNotClosedState("signalsPerPageChanged"))
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
#ifndef QT4_PORTED
    signal_height = (int32)(signal_browser_->getCanvasView()->visibleHeight() / tmp) -
                    signal_browser_model_->getSignalSpacing();
    signal_browser_model_->setSignalHeight(signal_height);
#else
    // TODO: move this calculation into signal_browser_model!
    signal_height = (int32)(signal_browser_->getVisibleHeight() / tmp) -
                    signal_browser_model_->getSignalSpacing();
    signal_browser_model_->setSignalHeight(signal_height);
#endif
}

// pixel per sec changed
void MainWindowModel::pixelPerSecChanged(float64 pixel_per_sec)
{
    if (!checkMainWindowPtr("pixelPerSecChanged") ||
        !checkNotClosedState("pixelPerSecChanged"))
    {
        return;
    }

#ifndef QT4_PORTED
    float64 secs_per_page = signal_browser_->getCanvasView()->visibleWidth() /
                            pixel_per_sec;
#else
    float64 secs_per_page = signal_browser_->getVisibleWidth() /
                            pixel_per_sec;
#endif

    secs_per_page = (int32)(secs_per_page * 10) / 10.0;
    main_window_->setSecsPerPage(secs_per_page);
}

// signal height changed
void MainWindowModel::signalHeightChanged(int32 signal_height)
{
    if (!checkMainWindowPtr("signalHeightChanged") ||
        !checkNotClosedState("signalHeightChanged"))
    {
        return;
    }

    float64 signals_per_page;
#ifndef QT4_PORTED
    signals_per_page = signal_browser_->getCanvasView()->visibleHeight() /
                       (float64)(signal_height +
                                 signal_browser_model_->getSignalSpacing());
#else
    signals_per_page = signal_browser_->getVisibleHeight() /
                       (float64)(signal_height +
                                 signal_browser_model_->getSignalSpacing());
#endif

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

    if (state_ != STATE_FILE_OPENED && state_ != STATE_FILE_CHANGED)
    {
        *log_stream_ << "MainWindowModel::setChanged"
                     << " Error: illegal state\n";
        return;
    }

    if (state_ != STATE_FILE_CHANGED)
    {
        setState(STATE_FILE_CHANGED);
    }
}

} // namespace BioSig_

