// file_signal_writer_factory.h

#ifndef FILE_SIGNAL_WRITER_FACTORY_H
#define FILE_SIGNAL_WRITER_FACTORY_H

#include "file_signal_writer.h"
#include "file_handler_factory.h"

#include <memory>

namespace BioSig_
{

// file signal reader Factory as singleton
class FileSignalWriterFactory : public FileHandlerFactory<FileSignalWriter>
{
public:
    static FileSignalWriterFactory* getInstance();

private:
     static std::auto_ptr<FileSignalWriterFactory> instance_;
};

} // namespace BioSig_

#endif
