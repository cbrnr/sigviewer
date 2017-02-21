// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "file_signal_reader_factory.h"

namespace sigviewer
{

//-------------------------------------------------------------------------
FileSignalReaderFactory* FileSignalReaderFactory::getInstance ()
{
    //auto_ptr is deprecated
    static std::unique_ptr<FileSignalReaderFactory> instance;
    if (!instance.get())
        instance.reset(new FileSignalReaderFactory);
    return instance.get();
}

}
