#ifndef GDF_WRITER_H
#define GDF_WRITER_H

#include "file_handling/file_signal_writer.h"

namespace BioSig_
{

//-----------------------------------------------------------------------------
class GDFWriter : public FileSignalWriter
{
public:
    GDFWriter();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<FileSignalWriter> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual bool supportsSavingEvents () const {return true;}

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile (QSharedPointer<EventManager const>,
                                            std::set<EventType> const& types);

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<EventManager> event_manager,
                          QString const& source_file_path,
                          std::set<EventType> const& types);
private:
    Q_DISABLE_COPY(GDFWriter)
};

}

#endif // GDF_WRITER_H
