// © SigViewer developers
//
// License: GPL-3.0

#include "file_signal_writer_factory.h"

namespace sigviewer {

// get instance
FileSignalWriterFactory* FileSignalWriterFactory::getInstance() {
    // auto_ptr is deprecated
    static std::unique_ptr<FileSignalWriterFactory> instance;
    if (!instance.get()) instance.reset(new FileSignalWriterFactory);
    return instance.get();
}

}  // namespace sigviewer
