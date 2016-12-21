// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef FILE_SIGNAL_WRITER_FACTORY_H
#define FILE_SIGNAL_WRITER_FACTORY_H

#include "file_signal_writer.h"
#include "file_handler_factory.h"

#include <memory>

namespace sigviewer
{

// file signal reader Factory as singleton
class FileSignalWriterFactory : public FileHandlerFactory<FileSignalWriter>
{
public:
    static FileSignalWriterFactory* getInstance();
};

}

#endif
