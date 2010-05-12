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
    virtual QSharedPointer<FileSignalWriter> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual ~BioSigWriter() {}

    //-------------------------------------------------------------------------
    virtual bool supportsSavingEvents () const;

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile (QSharedPointer<EventManager>,
                                            std::set<EventType> const& types);

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<EventManager> event_manager,
                          QString const& source_file_path,
                          std::set<EventType> const& types);

private:
    //-------------------------------------------------------------------------
    BioSigWriter (FileFormat target_type, QString new_file_path);

    static BioSigWriter prototype_instance_;

    FileFormat target_type_;
    QString new_file_path_;
    std::set<FileFormat> file_formats_support_event_saving_;
    mutable QMutex mutex_;
};

}

#endif /*BIOIGWRITER_H_*/
