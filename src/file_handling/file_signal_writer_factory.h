// file_signal_writer_factory.h

#ifndef FILE_SIGNAL_WRITER_FACTORY_H
#define FILE_SIGNAL_WRITER_FACTORY_H

#include "file_signal_writer.h"
#include "../base/prototype_factory.h"

#include <memory>

namespace BioSig_
{

// file signal reader Factory as singleton
class FileSignalWriterFactory : public PrototypeFactory<FileSignalWriter>
{
public:
    static FileSignalWriterFactory* getInstance();

    QString getExtensions();

private:
     static std::auto_ptr<FileSignalWriterFactory> instance_;
};

} // namespace BioSig_

#endif
