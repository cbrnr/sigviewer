#include "open_file_gui_command.h"
#include "../file_handling/file_signal_reader_factory.h"
#include "../signal_browser/signal_browser_model_4.h"
#include "../file_handling_impl/event_manager_impl.h"
#include "../file_handling_impl/channel_manager_impl.h"
#include "../tab_context.h"
#include "../file_context.h"
#include "../application_context.h"
#include "../gui_action_manager.h"
#include "../main_window_model.h"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

namespace BioSig_
{


//-----------------------------------------------------------------------------
GuiActionFactoryRegistrator OpenFileGuiCommand::registrator_ ("Open File",
                                                              QSharedPointer<OpenFileGuiCommand> (new OpenFileGuiCommand));


//-----------------------------------------------------------------------------
OpenFileGuiCommand::OpenFileGuiCommand ()
    : GuiActionCommand ("Open... (new impl)")
{
}

//-----------------------------------------------------------------------------
QSharedPointer<OpenFileGuiCommand> OpenFileGuiCommand::getInstance ()
{
    return QSharedPointer<OpenFileGuiCommand> (new OpenFileGuiCommand);
}


//-----------------------------------------------------------------------------
void OpenFileGuiCommand::init ()
{
    //getQAction()->setShortcut (QKeySequence::Open);
    getQAction()->setIcon (QIcon(":/images/icons/fileopen.png"));
}

//-----------------------------------------------------------------------------
void OpenFileGuiCommand::openFile (QString const& file_path)
{
    // close
    //if (file_context_)
    //{
    //    fileCloseAction();
    //    if (application_context_.getState() != APP_STATE_NO_FILE_OPEN)
    //        return; // user cancel
    //}

    QSharedPointer<FileSignalReader> file_signal_reader =
            createAndOpenFileSignalReader (file_path);

    if (file_signal_reader.isNull())
        return;

    QString file_name = file_path.section (QDir::separator(), -1);

    QSharedPointer<EventManager> event_manager (new EventManagerImpl (file_signal_reader));
    QSharedPointer<ChannelManager> channel_manager (new ChannelManagerImpl (file_signal_reader));
    QSharedPointer<FileContext> file_context (new FileContext (file_path, event_manager,
                                                 channel_manager, file_signal_reader));
    ApplicationContext::getInstance()->getGUIActionManager()->
            connect(file_context.data(), SIGNAL(stateChanged(FileState)), SLOT(setFileState(FileState)));

    QSettings settings("SigViewer");
    settings.setValue("file_open_path", file_path.left (file_path.length() -
                                                        file_name.length()));

    // initialize signal browser
    QSharedPointer<SignalBrowserModel> signal_browser_model =
            ApplicationContext::getInstance()->getMainWindowModel()->createSignalVisualisationOfFile (file_context);

    signal_browser_model->connect (event_manager.data(), SIGNAL(eventCreated(QSharedPointer<SignalEvent const>)),
                                   SLOT(addEventItem(QSharedPointer<SignalEvent const>)));
    signal_browser_model->connect (event_manager.data(), SIGNAL(eventRemoved(EventID)),
                                   SLOT(removeEventItem(EventID)));
    signal_browser_model->connect (event_manager.data(), SIGNAL(eventChanged(EventID)),
                                   SLOT(setEventChanged(EventID)));

    ApplicationContext::getInstance()->setState (APP_STATE_FILE_OPEN);

    std::set<ChannelID> shown_channels;
    shown_channels.insert (1);
    signal_browser_model->setShownChannels (shown_channels);
    signal_browser_model->updateLayout();
    ApplicationContext::getInstance()->addFileContext (file_context);
    /*  // select channels
    std::set<ChannelID> shown_channels = channelSelection();
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
    main_window_->setWindowTitle (file_name + tr(" - SigViewer"));*/
}

//-----------------------------------------------------------------------------
void OpenFileGuiCommand::trigger ()
{
    QString extensions = FileSignalReaderFactory::getInstance()->getExtensions();
    QSettings settings ("SigViewer");
    QString open_path = settings.value("file_open_path", ".").toString();
    if (!open_path.length())
        open_path = QDir::homePath ();
    QString file_path = showOpenDialog (open_path, extensions);

    if (file_path.isEmpty())
        return;

    openFile (file_path);
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
        extension_selection += ";; " + *it +" (" + *it + ")";
    }
    extension_selection += ";; *.* (*.*)";
    return QFileDialog::getOpenFileName (0, tr("Chose signal file to open"),
                                        path, extension_selection);
}

//-----------------------------------------------------------------------------
QSharedPointer<FileSignalReader> OpenFileGuiCommand::createAndOpenFileSignalReader
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
            load = signal_reader->open (file_path, false); // OVERFLOW DETECTION????

            if (load.size ())
            {
                QMessageBox::critical (0, tr("Error reading file"),
                                      tr("Invalid file: %1").arg (file_path+load));
            }
            else
                return signal_reader;
        }
    }
    return QSharedPointer<FileSignalReader> (0);
}



} // namespace BioSig_
