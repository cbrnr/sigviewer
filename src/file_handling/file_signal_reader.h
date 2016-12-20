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


#ifndef FILE_SIGNAL_READER_H
#define FILE_SIGNAL_READER_H

#include "basic_header.h"
#include "base/signal_event.h"
#include "base/data_block.h"

#include <QVector>
#include <QPointer>
#include <QSharedPointer>
#include <QPair>

class QString;

namespace sigviewer
{

//-----------------------------------------------------------------------------
/// FileSignalReader
///
/// abstract base class for reading signals from a file
class FileSignalReader
{
public:
    typedef QVector<SignalEvent> SignalEventVector;

    virtual ~FileSignalReader() {}

    virtual QPair<FileSignalReader*, QString> createInstance (QString const& file_path) = 0;

    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID channel_id,
                                                           size_t start_sample,
                                                           size_t length) const = 0;

    virtual QList<QSharedPointer<SignalEvent const> > getEvents () const = 0;

    virtual QSharedPointer<BasicHeader> getBasicHeader () = 0;

    virtual QSharedPointer<BasicHeader const> getBasicHeader () const = 0;

protected:
    FileSignalReader () {}

private:
    Q_DISABLE_COPY(FileSignalReader)
};

} // namespace SigViewer_

#endif
