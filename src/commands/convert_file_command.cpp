#include "convert_file_command.h"

#include "open_file_command.h"
#include "file_handling/file_signal_writer_factory.h"
#include "gui/progress_bar.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
QString ConvertFileCommand::execute ()
{
    if (destination_file_path_.size() == 0)
        return "Error: no output-file defined";

    OpenFileCommand open_file_command (application_context_, source_file_path_);
    QString open_error = open_file_command.execute ();
    if (open_error.size ())
        return open_error;

    QSharedPointer<FileSignalWriter> writer (FileSignalWriterFactory::getInstance()->getHandler (destination_file_path_));
    ProgressBar::instance().initAndShow (application_context_->getCurrentFileContext()->getChannelManager().getNumberSamples() +
                                         application_context_->getCurrentFileContext()->getEventManager()->getNumberOfEvents(), "Converting",
                                         application_context_);
    QString result = writer->save (application_context_->getCurrentFileContext());
    ProgressBar::instance().close ();

    return result;
}

}
