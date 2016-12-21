// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef CONVERT_FILE_COMMAND_H
#define CONVERT_FILE_COMMAND_H

#include "gui/application_context.h"

#include <QString>

namespace sigviewer
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
