#ifndef OPEN_FILE_COMMAND_H
#define OPEN_FILE_COMMAND_H

#include <QString>

namespace BioSig_
{

class OpenFileCommand
{
public:
    OpenFileCommand (QString const& filename_and_path)
        : filename_and_path_ (filename_and_path),
          non_gui_mode_ (true)
    {}

    void execute ();

private:
    void openFileInNonGuiMode ();

    QString filename_and_path_;
    bool instantly_;
    bool non_gui_mode_;
};

}

#endif // OPEN_FILE_COMMAND_H
