// evt_writer.h

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
