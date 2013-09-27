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


#ifndef CONVERT_FILE_COMMAND_H
#define CONVERT_FILE_COMMAND_H

#include "gui/application_context.h"

#include <QString>

namespace SigViewer_
{

class ConvertFileCommand
{
public:
    ConvertFileCommand (QSharedPointer<ApplicationContext> application_context,
                        QString const& source_file_path,
                        QString const& destination_file_path) :
        application_context_ (application_context),
        source_file_path_ (source_file_path),
        destination_file_path_ (destination_file_path)
    {}

    QString execute ();

private:
    QSharedPointer<ApplicationContext> application_context_;
    QString source_file_path_;
    QString destination_file_path_;
};

}

#endif // CONVERT_FILE_COMMAND_H
