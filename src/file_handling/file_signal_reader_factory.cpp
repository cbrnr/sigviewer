#include "file_signal_reader_factory.h"

namespace SigViewer_
{

//-------------------------------------------------------------------------
std::auto_ptr<FileSignalReaderFactory> FileSignalReaderFactory::instance_;

//-------------------------------------------------------------------------
FileSignalReaderFactory* FileSignalReaderFactory::getInstance ()
{
    if (!instance_.get())
    {
        instance_.reset(new FileSignalReaderFactory);
    }
    return instance_.get();
}

} // namespace SigViewer_
