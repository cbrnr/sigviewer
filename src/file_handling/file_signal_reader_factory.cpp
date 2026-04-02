// © SigViewer developers
//
// License: GPL-3.0

#include "file_signal_reader_factory.h"

namespace sigviewer {

//-------------------------------------------------------------------------
FileSignalReaderFactory* FileSignalReaderFactory::getInstance() {
    // auto_ptr is deprecated
    static std::unique_ptr<FileSignalReaderFactory> instance;
    if (!instance.get()) instance.reset(new FileSignalReaderFactory);
    return instance.get();
}

}  // namespace sigviewer
