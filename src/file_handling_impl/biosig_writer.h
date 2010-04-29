#ifndef BIOIGWRITER_H_
#define BIOIGWRITER_H_

#include "../file_handling/file_signal_writer.h"

#include <biosig.h>

#include <QString>
#include <QMutex>
#include <set>

namespace BioSig_ 
{

class BioSigWriter : public FileSignalWriter
{
public:
    //-------------------------------------------------------------------------
    BioSigWriter (FileFormat target_type);

    //-------------------------------------------------------------------------
    BioSigWriter (bool prototype_instance);

    //-------------------------------------------------------------------------
    virtual ~BioSigWriter();

    //-------------------------------------------------------------------------
    virtual FileSignalWriter* clone();

    //-------------------------------------------------------------------------
    virtual bool supportsSavingEvents () const;

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile (QSharedPointer<EventManager>,
                                            QString const& file_path);

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<EventManager> event_manager,
                          QString const& old_file_path,
                          QString const& file_path);

private:
    static BioSigWriter prototype_instance_;

    FileFormat target_type_;
    std::set<FileFormat> file_formats_support_event_saving_;
    mutable QMutex mutex_;
};

}

#endif /*BIOIGWRITER_H_*/
