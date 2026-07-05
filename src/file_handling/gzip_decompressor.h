// © SigViewer developers
//
// License: GPL-3.0


#ifndef GZIP_DECOMPRESSOR_H
#define GZIP_DECOMPRESSOR_H

#include <QString>

class QTemporaryFile;

namespace sigviewer
{

//-----------------------------------------------------------------------------
/// True if the file starts with the gzip magic bytes (0x1f 0x8b), regardless
/// of its extension.
bool isGzipCompressed (QString const& file_path);

//-----------------------------------------------------------------------------
/// Decompresses a gzip-compressed file into temp_file and returns
/// temp_file's path, or an empty string on failure. temp_file must stay in
/// scope for as long as the decompressed path is used; it removes the file
/// on destruction.
QString decompressGzip (QString const& gz_file_path, QTemporaryFile& temp_file);

}

#endif
