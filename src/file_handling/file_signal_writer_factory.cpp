// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "file_signal_writer_factory.h"

namespace sigviewer
{

// get instance
FileSignalWriterFactory* FileSignalWriterFactory::getInstance()
{
    static std::auto_ptr<FileSignalWriterFactory> instance;
    if (!instance.get())
        instance.reset(new FileSignalWriterFactory);
    return instance.get();
}

}
