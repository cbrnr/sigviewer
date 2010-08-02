// file_signal_reader_factory.h

#ifndef FILE_SIGNAL_READER_FACTORY_H
#define FILE_SIGNAL_READER_FACTORY_H

#include "file_handler_factory.h"
#include "file_signal_reader.h"

#include <QString>
#include <map>
#include <memory>

namespace SigViewer_
{

//-------------------------------------------------------------------------
// file signal reader Factory as singleton
class FileSignalReaderFactory : public FileHandlerFactory<FileSignalReader>
{
public:
    static FileSignalReaderFactory* getInstance ();

private:
    static std::auto_ptr<FileSignalReaderFactory> instance_;
};

} // namespace SigViewer_

#endif
