// © SigViewer developers
//
// License: GPL-3.0

#ifndef FILE_SIGNAL_WRITER_FACTORY_H
#define FILE_SIGNAL_WRITER_FACTORY_H

#include <memory>

#include "file_handler_factory.h"
#include "file_signal_writer.h"

namespace sigviewer {

// file signal reader Factory as singleton
class FileSignalWriterFactory : public FileHandlerFactory<FileSignalWriter> {
   public:
    static FileSignalWriterFactory* getInstance();
};

}  // namespace sigviewer

#endif
