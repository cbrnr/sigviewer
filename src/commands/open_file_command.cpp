#include "open_file_command.h"

#include "file_handling/file_signal_reader_factory.h"
#include "file_handling/channel_manager.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_context.h"
#include "application_context_impl.h"

#include <QDir>
#include <QSettings>

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString OpenFileCommand::execute ()
{
    QString file_path = QDir::toNativeSeparators (filename_and_path_);
    QSharedPointer<FileSignalReader> file_signal_reader = FileSignalReaderFactory::getInstance()->getHandler (file_path);
    if (file_signal_reader.isNull())
        return QString ("Can't open file ") + file_path;

    QSharedPointer<ChannelManager> channel_manager (new ChannelManagerImpl (file_signal_reader));
    QSharedPointer<EventManager> event_manager (new EventManagerImpl (file_signal_reader));
    QSharedPointer<FileContext> file_context (new FileContext (file_path, event_manager,
                                                 channel_manager, file_signal_reader->getBasicHeader()));
    ApplicationContextImpl::getInstance()->addFileContext (file_context);
    return "";
}

}
