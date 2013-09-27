// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


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
