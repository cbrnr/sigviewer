#include "file_signal_reader_factory.h"

namespace SigViewer_
{

//-------------------------------------------------------------------------
FileSignalReaderFactory* FileSignalReaderFactory::getInstance ()
{
    static std::auto_ptr<FileSignalReaderFactory> instance;
    if (!instance.get())
        instance.reset(new FileSignalReaderFactory);
    return instance.get();
}

} // namespace SigViewer_
