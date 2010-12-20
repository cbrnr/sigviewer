#ifndef GDF_WRITER_H
#define GDF_WRITER_H

#include "file_handling/file_signal_writer.h"

namespace SigViewer_
{

//-----------------------------------------------------------------------------
class GDFFileSignalWriter : public FileSignalWriter
{
public:
    //-------------------------------------------------------------------------
    GDFFileSignalWriter ();

    //-------------------------------------------------------------------------
    virtual QPair<FileSignalWriter*, QString> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual bool supportsSavingEvents () const {return true;}

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile (QSharedPointer<EventManager const>,
                                            std::set<EventType> const& types);

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<FileContext const> file_context,
                          std::set<EventType> const& types);
private:
    GDFFileSignalWriter (QString const& file_path);

    QString new_file_path_;
    Q_DISABLE_COPY(GDFFileSignalWriter)
};

}

#endif // GDF_WRITER_H
