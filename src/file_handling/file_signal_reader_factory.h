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

} // namespace SigViewer_

#endif
