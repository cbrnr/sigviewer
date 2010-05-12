#include "file_signal_writer_factory.h"

namespace BioSig_
{

// instance
std::auto_ptr<FileSignalWriterFactory> FileSignalWriterFactory::instance_;

// get instance
FileSignalWriterFactory* FileSignalWriterFactory::getInstance()
{
    if (!instance_.get())
    {
        instance_.reset(new FileSignalWriterFactory);
    }
    return instance_.get();
}

} // namespace BioSig_
