#include "file_signal_writer_factory.h"

namespace SigViewer_
{

// get instance
FileSignalWriterFactory* FileSignalWriterFactory::getInstance()
{
    static std::auto_ptr<FileSignalWriterFactory> instance;
    if (!instance.get())
        instance.reset(new FileSignalWriterFactory);
    return instance.get();
}

} // namespace SigViewer_
