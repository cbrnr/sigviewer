// © SigViewer developers
//
// License: GPL-3.0


#include "gzip_decompressor.h"

#include <QFile>
#include <QTemporaryFile>

#include <zlib.h>

namespace sigviewer
{

//-----------------------------------------------------------------------------
bool isGzipCompressed (QString const& file_path)
{
    QFile file (file_path);
    if (!file.open (QIODevice::ReadOnly))
        return false;

    unsigned char magic[2] = {0, 0};
    file.read (reinterpret_cast<char*> (magic), sizeof (magic));
    return magic[0] == 0x1f && magic[1] == 0x8b;
}

//-----------------------------------------------------------------------------
QString decompressGzip (QString const& gz_file_path, QTemporaryFile& temp_file)
{
    gzFile gz = gzopen (gz_file_path.toLocal8Bit().constData(), "rb");
    if (!gz)
        return QString();

    if (!temp_file.open())
    {
        gzclose (gz);
        return QString();
    }

    char buffer[65536];
    int bytes_read;
    bool ok = true;
    while ((bytes_read = gzread (gz, buffer, sizeof (buffer))) > 0)
    {
        if (temp_file.write (buffer, bytes_read) != bytes_read)
        {
            ok = false;
            break;
        }
    }

    int gz_error = Z_OK;
    gzerror (gz, &gz_error);
    if (bytes_read < 0 || gz_error != Z_OK)
        ok = false;

    gzclose (gz);
    temp_file.close();

    return ok ? temp_file.fileName() : QString();
}

}
