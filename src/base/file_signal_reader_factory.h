// file_signal_reader_factory.h

#ifndef FILE_SIGNAL_READER_FACTORY_H
#define FILE_SIGNAL_READER_FACTORY_H

#include "prototype_factory.h"
#include "file_signal_reader.h"

#include <memory>

namespace BioSig_
{

// file signal reader Factory as singleton
class FileSignalReaderFactory : public PrototypeFactory<FileSignalReader>
{
public:
    static FileSignalReaderFactory* getInstance();

    QString getExtensions();

private:
     static std::auto_ptr<FileSignalReaderFactory> instance_;
};

} // namespace BioSig_

#endif
