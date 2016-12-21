// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


// file_signal_reader_factory.h

#ifndef FILE_SIGNAL_READER_FACTORY_H
#define FILE_SIGNAL_READER_FACTORY_H

#include "file_handler_factory.h"
#include "file_signal_reader.h"

#include <QString>
#include <map>
#include <memory>

namespace sigviewer
{

//-------------------------------------------------------------------------
// file signal reader Factory as singleton
class FileSignalReaderFactory : public FileHandlerFactory<FileSignalReader>
{
public:
    static FileSignalReaderFactory* getInstance ();
};

}
#endif
