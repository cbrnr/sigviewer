// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef OPEN_FILE_COMMAND_H
#define OPEN_FILE_COMMAND_H

#include "gui/application_context.h"

#include <QString>

namespace sigviewer
{

class OpenFileCommand
{
public:
    OpenFileCommand (QSharedPointer<ApplicationContext> application_context,
                     QString const& filename_and_path)
        : application_context_ (application_context),
          filename_and_path_ (filename_and_path),
          non_gui_mode_ (true)
    {}

    QString execute ();

private:
    void openFileInNonGuiMode ();

    QSharedPointer<ApplicationContext> application_context_;
    QString filename_and_path_;
    bool instantly_;
    bool non_gui_mode_;
};

}

#endif // OPEN_FILE_COMMAND_H
