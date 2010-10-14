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

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
QString const OpenFileGuiCommand::IMPORT_EVENTS_ = "Import Events...";
QString const OpenFileGuiCommand::OPEN_ = "Open...";
QString const OpenFileGuiCommand::SHOW_FILE_INFO_ = "Info...";
QStringList const OpenFileGuiCommand::ACTIONS_ = QStringList() <<
                                                 OpenFileGuiCommand::IMPORT_EVENTS_ <<
                                                 OpenFileGuiCommand::OPEN_ <<
                                                 OpenFileGuiCommand::SHOW_FILE_INFO_;

//-----------------------------------------------------------------------------
QSharedPointer<OpenFileGuiCommand> OpenFileGuiCommand::instance_ = QSharedPointer<OpenFileGuiCommand> (new OpenFileGuiCommand);

//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator OpenFileGuiCommand::registrator_ ("Opening",
                                                              OpenFileGuiCommand::instance_);


//-----------------------------------------------------------------------------
OpenFileGuiCommand::OpenFileGuiCommand ()
    : GuiActionCommand (ACTIONS_)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
OpenFileGuiCommand::~OpenFileGuiCommand ()
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
void OpenFileGuiCommand::init ()
{
    setShortcut (OPEN_, QKeySequence::Open);
    setIcon (OPEN_, QIcon(":/images/icons/fileopen.png"));
    setIcon (SHOW_FILE_INFO_, QIcon(":/images/info_16x16.png"));

    resetActionTriggerSlot (OPEN_, SLOT(open()));
    resetActionTriggerSlot (IMPORT_EVENTS_, SLOT(importEvents()));
    resetActionTriggerSlot (SHOW_FILE_INFO_, SLOT(showFileInfo()));
}

//-----------------------------------------------------------------------------
void OpenFileGuiCommand::openFile (QString file_path, bool instantly)
{
    instance_->openFileImpl (file_path, instantly);
}

//-------------------------------------------------------------------------
void OpenFileGuiCommand::evaluateEnabledness ()
{
    bool file_opened = (getApplicationState() == APP_STATE_FILE_OPEN);
    getQAction (IMPORT_EVENTS_)->setEnabled (file_opened);
    getQAction (SHOW_FILE_INFO_)->setEnabled (file_opened);
}


//-------------------------------------------------------------------------
void OpenFileGuiCommand::open ()
{
    QStringList extension_list = FileSignalReaderFactory::getInstance()->getAllFileEndingsWithWildcards();
    QString extensions;
    foreach (QString extension, extension_list)
        extensions.append (extension + " ");
    QSettings settings ("SigViewer");
    QString open_path = settings.value ("file_open_path").toString();
    if (!open_path.length())
        open_path = QDir::homePath ();
    QString file_path = showOpenDialog (open_path, extensions);

    if (file_path.isEmpty())
        return;

    openFileImpl (file_path);
}

//-------------------------------------------------------------------------
void OpenFileGuiCommand::importEvents ()
{
    QString extensions = "*.evt";
    QSettings settings ("SigViewer");
    QString open_path = settings.value ("file_open_path").toString();
    if (!open_path.length())
        open_path = QDir::homePath ();
    QString file_path = showOpenDialog (open_path, extensions);

    FileSignalReader* file_signal_reader = FileSignalReaderFactory::getInstance()->getHandler (file_path);

    if (file_signal_reader == 0)
        return;

    QList<QSharedPointer<SignalEvent const> > events = file_signal_reader->getEvents ();

    QSharedPointer<EventManager> event_manager = currentVisModel()->getEventManager();
    QList<QSharedPointer<QUndoCommand> > creation_commands;
    foreach (QSharedPointer<SignalEvent const> event, events)
    {
        QSharedPointer<QUndoCommand> creation_command (new NewEventUndoCommand (event_manager, event));
        creation_commands.append (creation_command);
    }
    MacroUndoCommand* macro_command = new MacroUndoCommand (creation_commands);
    applicationContext()->getCurrentCommandExecuter()->executeCommand (macro_command);
    delete file_signal_reader;
}

//-------------------------------------------------------------------------
void OpenFileGuiCommand::showFileInfo ()
{
    BasicHeaderInfoDialog basic_header_info_dialog(applicationContext()->getCurrentFileContext()->getHeader());

    basic_header_info_dialog.loadSettings();
    basic_header_info_dialog.exec();
    basic_header_info_dialog.saveSettings();
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

    QSettings settings("SigViewer");
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

} // namespace SigViewer_
