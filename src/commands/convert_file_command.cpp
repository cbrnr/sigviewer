#include "convert_file_command.h"

#include "open_file_command.h"
#include "file_handling/file_signal_writer_factory.h"
#include "application_context_impl.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
QString ConvertFileCommand::execute ()
{
    OpenFileCommand open_file_command (source_file_path_);
    open_file_command.execute ();

    QSharedPointer<FileSignalWriter> writer = FileSignalWriterFactory::getInstance()->getHandler (destination_file_path_);
    return writer->save (ApplicationContextImpl::getInstance()->getCurrentFileContext());
}

}
