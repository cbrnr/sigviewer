// file_signal_reader_factory.h

#include "file_signal_reader_factory.h"

// all suported readers
#include "gdf/gdf_reader.h"
#include "bkr/bkr_reader.h"

namespace BioSig_
{

// instance
std::auto_ptr<FileSignalReaderFactory> FileSignalReaderFactory::instance_;

// get instance
FileSignalReaderFactory* FileSignalReaderFactory::getInstance()
{
    if (!instance_.get())
    {
        instance_.reset(new FileSignalReaderFactory);

        // register all readers
        instance_->addPrototype(".evt", new GDFReader);
        instance_->addPrototype(".gdf", new GDFReader);
        
        // TODO: uncomment as soon as BKRReader is finished
        //instance_->addPrototype(".bkr", new BKRReader);
    }
    return instance_.get();
}

// get extensions
QString FileSignalReaderFactory::getExtensions()
{
    QString extensions;
    QStringList extension_list = getElementNames();
    foreach(QString extension, extension_list)
    {
        if (extension.startsWith("."))
        {
            extensions += "*" + extension + " ";
        }
    }
    if (extensions.length() > 0)
    {
        extensions = extensions.left(extensions.length() - 1);
    }
    return extensions;
}

} // namespace BioSig_
