// © SigViewer developers
//
// License: GPL-3.0

// file_signal_reader_factory.h

#ifndef FILE_SIGNAL_READER_FACTORY_H
#define FILE_SIGNAL_READER_FACTORY_H

#include <QString>
#include <map>
#include <memory>

#include "file_handler_factory.h"
#include "file_signal_reader.h"

namespace sigviewer {

//-------------------------------------------------------------------------
// file signal reader Factory as singleton
class FileSignalReaderFactory : public FileHandlerFactory<FileSignalReader> {
   public:
    static FileSignalReaderFactory* getInstance();
};

}  // namespace sigviewer
#endif
