#ifndef CONVERT_FILE_COMMAND_H
#define CONVERT_FILE_COMMAND_H

#include <QString>

namespace SigViewer_
{

class ConvertFileCommand
{
public:
    ConvertFileCommand (QString const& source_file_path,
                        QString const& destination_file_path) :
        source_file_path_ (source_file_path),
        destination_file_path_ (destination_file_path)
    {}

    QString execute ();

private:
    QString source_file_path_;
    QString destination_file_path_;
};

}

#endif // CONVERT_FILE_COMMAND_H
