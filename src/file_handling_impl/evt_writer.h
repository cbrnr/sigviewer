// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef EVT_WRITER_H
#define EVT_WRITER_H

#include "file_handling/file_signal_writer.h"

#include <QFile>

namespace sigviewer
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
                                            std::set<EventType> const&) {return QObject::tr("not implemented!");}

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

}

#endif
