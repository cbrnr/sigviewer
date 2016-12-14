// file_signal_writer_factory.h

#ifndef FILE_SIGNAL_WRITER_FACTORY_H
#define FILE_SIGNAL_WRITER_FACTORY_H

#include "file_signal_writer.h"
#include "file_handler_factory.h"

#include <memory>

namespace SigViewer_
{

// file signal reader Factory as singleton
class FileSignalWriterFactory : public FileHandlerFactory<FileSignalWriter>
{
public:
    static FileSignalWriterFactory* getInstance();
};

} // namespace SigViewer_

#endif
