// file_signal_writer.h

#ifndef FILE_SIGNAL_WRITER_H
#define FILE_SIGNAL_WRITER_H

#include "file_signal_reader.h"
#include "event_manager.h"
#include "channel_manager.h"
#include "../base/signal_event.h"



class QTextStream;

namespace BioSig_
{

// abstract signal writer
class FileSignalWriter
{
public:
    typedef FileSignalReader::SignalEventVector SignalEventVector;

    virtual ~FileSignalWriter();
    virtual FileSignalWriter* clone() = 0;

    void setLogStream(QTextStream* log_stream);

    //-------------------------------------------------------------------------
    virtual bool supportsSavingEvents () const {return false;}

    //-------------------------------------------------------------------------
    virtual QString saveEventsToSignalFile (QSharedPointer<EventManager>,
                                QString const& file_path) {return "not implemented";}

    //-------------------------------------------------------------------------
    virtual QString save (QSharedPointer<EventManager> event_manager,
                             QString const& old_file_path,
                             QString const& file_path) {return "not implemented yet!";}

protected: 
    FileSignalWriter();

    QTextStream* log_stream_;

private:
    // not allowed
    FileSignalWriter(const FileSignalWriter&);
    const FileSignalWriter& operator=(const FileSignalWriter&);
};

} // namespace BioSig_

#endif
