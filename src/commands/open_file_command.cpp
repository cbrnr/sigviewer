// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


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
