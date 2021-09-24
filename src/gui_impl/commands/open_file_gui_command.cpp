// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl

#include <biosig.h>
#include "open_file_gui_command.h"
#include "gui_impl/gui_helper_functions.h"

#include "gui_impl/dialogs/basic_header_info_dialog.h"
#include "gui/signal_visualisation_model.h"
#include "file_handling/file_signal_reader_factory.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "tab_context.h"
#include "file_context.h"
#include "gui/main_window_model.h"
#include "editing_commands/macro_undo_command.h"
#include "editing_commands/new_event_undo_command.h"
#include "gui/progress_bar.h"
#include "gui/color_manager.h"
#include "close_file_gui_command.h"
#include "file_handling_impl/xdf_reader.h"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QCheckBox>

#include <algorithm>
#include <fstream>

namespace sigviewer
{

//-----------------------------------------------------------------------------
namespace {

class OpenFileGuiCommandFactory: public GuiActionCommandFactory
{
public:
    QSharedPointer<GuiActionCommand> createCommand() override
    {
        return QSharedPointer<OpenFileGuiCommand> (new OpenFileGuiCommand);
    }
};

} // unnamed namespace

QString const OpenFileGuiCommand::IMPORT_EVENTS_()
{
    static QString value = tr("Import Events...");

    return value;
}

QString const OpenFileGuiCommand::OPEN_()
{
    static QString value = tr("Open...");

    return value;
}

QString const OpenFileGuiCommand::SHOW_FILE_INFO_()
{
    static QString value = tr("Info...");

    return value;
}

QStringList const OpenFileGuiCommand::ACTIONS_()
{
    static QStringList result = {
        OpenFileGuiCommand::IMPORT_EVENTS_(),
        OpenFileGuiCommand::OPEN_(),
        OpenFileGuiCommand::SHOW_FILE_INFO_(),
    };

    return result;
}

//-----------------------------------------------------------------------------
QSharedPointer<OpenFileGuiCommand> OpenFileGuiCommand::instance_()
{
    static QSharedPointer<OpenFileGuiCommand> value{new OpenFileGuiCommand};

    return value;
}

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator OpenFileGuiCommand::registrator_ ("Opening",
                                                              QSharedPointer<OpenFileGuiCommandFactory> (new OpenFileGuiCommandFactory));


//-----------------------------------------------------------------------------
OpenFileGuiCommand::OpenFileGuiCommand ()
    : GuiActionCommand (ACTIONS_())
{
    QSettings settings;
    do_not_show_warning_message = settings.value("DoNotShowWarningMessage", false).toBool();
}

//-------------------------------------------------------------------------
OpenFileGuiCommand::~OpenFileGuiCommand ()
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
void OpenFileGuiCommand::init ()
{
    setShortcut (OPEN_(), QKeySequence::Open);
    setShortcut (SHOW_FILE_INFO_(), tr("Ctrl+I"));
    setIcon (OPEN_(), QIcon(":/images/ic_folder_open_black_24dp.png"));
    setIcon (SHOW_FILE_INFO_(), QIcon(":/images/ic_info_outline_black_24dp.png"));
    setIcon (IMPORT_EVENTS_(), QIcon(":/images/ic_file_download_black_24dp.png"));


    resetActionTriggerSlot (OPEN_(), SLOT(open()));
    resetActionTriggerSlot (IMPORT_EVENTS_(), SLOT(importEvents()));
    resetActionTriggerSlot (SHOW_FILE_INFO_(), SLOT(showFileInfo()));
}

//-----------------------------------------------------------------------------
void OpenFileGuiCommand::openFile (QString file_path)
{
    if (!instance_()->confirmClosingOldFile())   /*!< In case the user decides not to close the old file, return. */
        return;

    //close the previous file before opening a new one
    //thus using less memory
    CloseFileGuiCommand closeObject;
    if (closeObject.closeCurrentFile())
    {
        QSettings settings;
        settings.setValue("autoScaling", true);

        instance_()->openFileImpl (file_path);
    }
}

//-------------------------------------------------------------------------
bool OpenFileGuiCommand::confirmClosingOldFile()
{
    QSharedPointer<FileContext> current_file_context =
            applicationContext()->getCurrentFileContext();

    if (!current_file_context.isNull() &&
            current_file_context->getState () == FILE_STATE_CHANGED)
    {
        QString file_name = current_file_context->getFileName ();
        QMessageBox::StandardButton pressed_button
                =  QMessageBox::question (0, tr("Really close?"),
                                          tr("Changes in '%1' are not saved!!").arg(file_name) + "\n" +
                                          tr("Really close?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (pressed_button == QMessageBox::No)
            return false;
        else
            current_file_context->setState(FILE_STATE_UNCHANGED);
    }

    return true;
}

//-------------------------------------------------------------------------
void OpenFileGuiCommand::evaluateEnabledness ()
{
    bool file_opened = (getApplicationState() == APP_STATE_FILE_OPEN);
    bool enable_import_events = file_opened;

    if (file_opened)
    {
        if (applicationContext()->getCurrentFileContext()->getFileName().endsWith("xdf"))
            enable_import_events = false;//Disabled because currently XDF files doesn't support importing events
    }

    getQAction (IMPORT_EVENTS_())->setEnabled (enable_import_events);
    getQAction (SHOW_FILE_INFO_())->setEnabled (file_opened);
}


//-------------------------------------------------------------------------
void OpenFileGuiCommand::open ()
{
    if (!instance_()->confirmClosingOldFile())   /*!< In case the user decides not to close the old file, return. */
        return;

    QStringList extension_list = FileSignalReaderFactory::getInstance()->getAllFileEndingsWithWildcards();
    QString extensions;
    foreach (QString extension, extension_list)
        extensions.append (extension + " ");
    QSettings settings;
    QString open_path = settings.value ("file_open_path").toString();
    if (!open_path.length())
        open_path = QDir::homePath ();
    QString file_path = showOpenDialog (open_path, extensions);

    if (file_path.isEmpty())
        return;

    //close the previous file before opening a new one
    //thus using less memory
    CloseFileGuiCommand closeObject;
    if (closeObject.closeCurrentFile())
    {
        QSettings settings;
        settings.setValue("autoScaling", true);

        instance_()->openFileImpl (file_path);
    }
}

//-------------------------------------------------------------------------
void OpenFileGuiCommand::importEvents ()
{
    QString extensions = "*.csv *.evt";
    QSettings settings;
    QString open_path = settings.value ("file_open_path").toString();
    if (!open_path.length())
        open_path = QDir::homePath ();
    QString file_path = showOpenDialog (open_path, extensions);

    if (file_path.isEmpty())
        return;

    QList<QSharedPointer<SignalEvent const> > events;
    QSharedPointer<EventManager> event_manager = applicationContext()->getCurrentFileContext()->getEventManager();
    double sampleRate = event_manager->getSampleRate();
    std::set<EventType> types = event_manager->getEventTypes();
    int numberChannels = applicationContext()->getCurrentFileContext()->getChannelManager().getNumberChannels();

    // try reading event file through biosig
    HDRTYPE* evtHDR = sopen(file_path.toStdString().c_str(), "r", NULL );
    if (!serror2(evtHDR)) {
        /* Note: evtSampleRate and transition rate can be NaN,
           indicating sample rate is not specified in event file
         */
	double evtSampleRate   = biosig_get_eventtable_samplerate(evtHDR);
	double transition_rate = sampleRate / evtSampleRate;
	size_t NumEvents       = biosig_get_number_of_events(evtHDR);
	for (size_t k = 0; k < NumEvents; k++) {
            uint16_t typ; uint32_t pos; uint16_t chn; uint32_t dur;
            gdf_time timestamp;
            const char *desc;
            biosig_get_nth_event(evtHDR, k, &typ, &pos, &chn, &dur, &timestamp, &desc);

            if (transition_rate > 0) {
                pos = lround(pos*transition_rate);
                dur = lround(dur*transition_rate);
            }

            /* biosig uses a 1-based channel index, and 0 refers to all channels,
               sigviewer uses a 0-based indexing, and -1 indicates all channels */
            //boundary check & error handling
            if (pos > event_manager->getMaxEventPosition()
                    || pos + dur > event_manager->getMaxEventPosition()
                    || chn > numberChannels
                    || !types.count(typ))
                continue;

            QSharedPointer<SignalEvent> event = QSharedPointer<SignalEvent>(new SignalEvent(pos,
                    typ, sampleRate, -1, chn-1, dur));

            events << event;
        }
        sclose(evtHDR);
        destructHDR(evtHDR);
    } else
    {
        // if the file can not be read with biosig, try this approach
        destructHDR(evtHDR);
        QMessageBox::critical(0, file_path, tr("Cannot open file.\nIs the target file open in another application?"));
        return;
    }

    QList<QSharedPointer<QUndoCommand> > creation_commands;
    foreach (QSharedPointer<SignalEvent const> event, events)
    {
            QSharedPointer<QUndoCommand> creation_command (new NewEventUndoCommand (event_manager, event));
            creation_commands.append (creation_command);
    }
    MacroUndoCommand* macro_command = new MacroUndoCommand (creation_commands);
    applicationContext()->getCurrentCommandExecuter()->executeCommand (macro_command);
}

//-------------------------------------------------------------------------
void OpenFileGuiCommand::showFileInfo ()
{
    basic_header_info_dialog = QSharedPointer<BasicHeaderInfoDialog>
            (new BasicHeaderInfoDialog (applicationContext()->getCurrentFileContext()->getHeader()));

    basic_header_info_dialog->show();
}

//-------------------------------------------------------------------------
void OpenFileGuiCommand::openFileImpl (QString file_path, bool instantly)
{
    file_path = QDir::toNativeSeparators (file_path);
    FileSignalReader* file_signal_reader = FileSignalReaderFactory::getInstance()->getHandler (file_path);

    if (file_signal_reader == 0)
        return;

    QString file_name = file_path.section (QDir::separator(), -1);

    ChannelManager* channel_manager (new ChannelManagerImpl (file_signal_reader));

    std::set<ChannelID> shown_channels;
    if (instantly)
        shown_channels = channel_manager->getChannels();
    else
        shown_channels = GuiHelper::selectChannels (*channel_manager,
                                                    applicationContext()->getEventColorManager(),
                                                    file_signal_reader->getBasicHeader());
    if (shown_channels.size() == 0)
    {
        delete channel_manager;
        return;
    }

    ProgressBar::instance().initAndShow (channel_manager->getNumberChannels() * 3, tr("Opening ") + file_name,
                                         applicationContext());
    QSharedPointer<EventManager> event_manager (new EventManagerImpl (*file_signal_reader));
    QSharedPointer<FileContext> file_context (new FileContext (file_path, event_manager,
                                                 channel_manager, file_signal_reader->getBasicHeader()));

    QSettings settings;
    settings.setValue("file_open_path", file_path.left (file_path.length() -
                                                        file_name.length()));

    QSharedPointer<SignalVisualisationModel> signal_visualisation_model =
            applicationContext()->getMainWindowModel()->createSignalVisualisationOfFile (file_context);

    signal_visualisation_model->setShownChannels (shown_channels);
    signal_visualisation_model->update();
    applicationContext()->addFileContext (file_context);
    ProgressBar::instance().close();
}


//-----------------------------------------------------------------------------
QString OpenFileGuiCommand::showOpenDialog (QString const& path, QString const& extensions)
{
    QString extension_selection = tr("Signal files (%1)").arg(extensions);
    QStringList ext_list = extensions.split (" ");
    for (QStringList::iterator it = ext_list.begin();
         it != ext_list.end();
         it++)
    {
        if (it->size ())
            extension_selection += ";; " + *it +" (" + *it + ")";
    }
    extension_selection += ";; *.* (*.*)";
    return QFileDialog::getOpenFileName (0, tr("Chose signal file to open"),
                                        path, extension_selection);
}

}
