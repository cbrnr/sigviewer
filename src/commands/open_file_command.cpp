#include "open_file_command.h"

#include "file_handling/file_signal_reader_factory.h"
#include "file_handling/channel_manager.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_context.h"

#include <QDir>
#include <QSettings>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
QString OpenFileCommand::execute ()
{
    QString file_path = QDir::toNativeSeparators (filename_and_path_);
    FileSignalReader* file_signal_reader (FileSignalReaderFactory::getInstance()->getHandler (file_path));
    if (file_signal_reader == 0)
        return QString ("Can't open file ") + file_path;

    ChannelManager* channel_manager (new ChannelManagerImpl (file_signal_reader));
    QSharedPointer<EventManager> event_manager (new EventManagerImpl (*file_signal_reader));
    QSharedPointer<FileContext> file_context (new FileContext (file_path, event_manager,
                                                 channel_manager, file_signal_reader->getBasicHeader()));
    application_context_->addFileContext (file_context);
    return "";
}

}
