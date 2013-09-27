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


#ifndef FILE_SIGNAL_WRITER_H
#define FILE_SIGNAL_WRITER_H

#include "file_signal_reader.h"
#include "file_context.h"
#include "event_manager.h"
#include "channel_manager.h"
#include "base/signal_event.h"

namespace SigViewer_
{

// abstract signal writer
class FileSignalWriter
{
public:
    virtual ~FileSignalWriter() {}

    //-------------------------------------------------------------------------
    virtual QPair<FileSignalWriter*, QString> createInstance (QString const& file_path) = 0;

    //-------------------------------------------------------------------------
    virtual bool supportsSavingEvents () const {return false;}

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile (QSharedPointer<EventManager const>,
                                            std::set<EventType> const& types) = 0;

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<FileContext const> file_context)
    {
        QSharedPointer<EventManager const> event_manager = file_context->getEventManager();
        if (event_manager.isNull())
            return save (file_context, std::set<EventType> ());
        else
            return save (file_context, event_manager->getEventTypes (""));
    }

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<FileContext const> file_context,
                          std::set<EventType> const& types) = 0;

protected: 
    FileSignalWriter () {}

private:
    Q_DISABLE_COPY(FileSignalWriter)
};

} // namespace SigViewer_

#endif
