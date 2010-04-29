// evt_writer.h

#ifndef EVT_WRITER_H
#define EVT_WRITER_H

#include "../file_handling/file_signal_writer.h"

#include <QFile>

namespace BioSig_
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
    virtual ~EVTWriter();

    //-------------------------------------------------------------------------
    virtual FileSignalWriter* clone();

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<EventManager> event_manager,
                          QString const& old_file_path,
                          QString const& file_path);

private:
    static EVTWriter prototype_instance_;

    // not allowed
    EVTWriter(const EVTWriter& src);
    const EVTWriter& operator=(const EVTWriter& src);
};

} // namespace BioSig_

#endif
