#include "open_file_gui_command.h"
#include "gui_helper_functions.h"

#include "../gui/signal_visualisation_model.h"
#include "../file_handling/file_signal_reader_factory.h"
#include "../file_handling_impl/event_manager_impl.h"
#include "../file_handling_impl/channel_manager_impl.h"
#include "../tab_context.h"
#include "../file_context.h"
#include "../application_context.h"
#include "../gui_action_manager.h"
#include "../gui/main_window_model.h"

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
    : GuiActionCommand ("Open...")
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
    //getQAction()->setShortcut (QKeySequence::Open);
    getQAction()->setIcon (QIcon(":/images/icons/fileopen.png"));
}

//-----------------------------------------------------------------------------
void OpenFileGuiCommand::openFile (QString const& file_path)
{
    QSharedPointer<FileSignalReader> file_signal_reader =
            createAndOpenFileSignalReader (file_path);

    if (file_signal_reader.isNull())
        return;

    QString file_name = file_path.section (QDir::separator(), -1);

    QSharedPointer<ChannelManager> channel_manager (new ChannelManagerImpl (file_signal_reader));

    std::set<ChannelID> shown_channels = GuiHelper::selectChannels (channel_manager,
                                                                    file_name);
    if (shown_channels.size() == 0)
        return;

    QSharedPointer<EventManager> event_manager (new EventManagerImpl (file_signal_reader));
    QSharedPointer<FileContext> file_context (new FileContext (file_path, event_manager,
                                                 channel_manager, file_signal_reader));

    QSettings settings("SigViewer");
    settings.setValue("file_open_path", file_path.left (file_path.length() -
                                                        file_name.length()));

    QSharedPointer<SignalVisualisationModel> signal_visualisation_model =
            ApplicationContext::getInstance()->getMainWindowModel()->createSignalVisualisationOfFile (file_context);

    signal_visualisation_model->setShownChannels (shown_channels);
    signal_visualisation_model->updateLayout();
    ApplicationContext::getInstance()->addFileContext (file_context);
    ApplicationContext::getInstance()->setState (APP_STATE_FILE_OPEN);
}

//-----------------------------------------------------------------------------
void OpenFileGuiCommand::trigger ()
{
    QString extensions = FileSignalReaderFactory::getInstance()->getExtensions();
    QSettings settings ("SigViewer");
    QString open_path = settings.value ("file_open_path").toString();
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
        (QString const& file_path)
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
