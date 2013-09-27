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


#ifndef EVT_WRITER_H
#define EVT_WRITER_H

#include "file_handling/file_signal_writer.h"

#include <QFile>

namespace SigViewer_
{

// EVT writer
class EVTWriter : public FileSignalWriter
{
public:
    //-------------------------------------------------------------------------
    EVTWriter ();

    //-------------------------------------------------------------------------
    EVTWriter (bool prototype_instance);

    //-------------------------------------------------------------------------
    virtual QPair<FileSignalWriter*, QString> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual ~EVTWriter();

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile (QSharedPointer<EventManager const>,
                                            std::set<EventType> const&) {return "not implemented!";}

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<FileContext const> file_context,
                          std::set<EventType> const& types);

private:
    //-------------------------------------------------------------------------
    EVTWriter (QString const& new_file_path);

    static EVTWriter prototype_instance_;
    QString new_file_path_;

    // not allowed
    Q_DISABLE_COPY(EVTWriter)
};

} // namespace SigViewer_

#endif
