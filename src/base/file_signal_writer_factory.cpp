// file_signal_writer_factory.h

#include "file_signal_writer_factory.h"

// all suported writers
#include "gdf/gdf_writer.h"

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

        // register all writers
        instance_->addPrototype(".evt", new GDFWriter);
        instance_->addPrototype(".gdf", new GDFWriter);
    }
    return instance_.get();
}

// get extensions
QString FileSignalWriterFactory::getExtensions()
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
