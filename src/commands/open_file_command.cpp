// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "open_file_command.h"

#include "file_handling/file_signal_reader_factory.h"
#include "file_handling_impl/channel_manager_impl.h"
#include "file_handling_impl/event_manager_impl.h"

#include <QDir>

namespace sigviewer
{

//-----------------------------------------------------------------------------
QString OpenFileCommand::execute ()
{
    QString file_path = QDir::toNativeSeparators (filename_and_path_);
    FileSignalReader* file_signal_reader (FileSignalReaderFactory::getInstance()->getHandler (file_path));
    if (file_signal_reader == 0)
        return QObject::tr("Can't open file %1").arg(file_path);

    ChannelManager* channel_manager (new ChannelManagerImpl (file_signal_reader));
    QSharedPointer<EventManager> event_manager (new EventManagerImpl (*file_signal_reader));
    QSharedPointer<FileContext> file_context (new FileContext (file_path, event_manager,
                                                 channel_manager, file_signal_reader->getBasicHeader()));
    application_context_->addFileContext (file_context);
    return "";
}

}
