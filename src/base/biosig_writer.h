#ifndef BIOIGWRITER_H_
#define BIOIGWRITER_H_

#include "file_signal_writer.h"

#include <biosig.h>

#include <QString>
#include <QMutex>
#include <set>

namespace BioSig_ 
{

class BioSigWriter : public FileSignalWriter
{
public:
    BioSigWriter(FileFormat target_type);
    virtual ~BioSigWriter();

    virtual FileSignalWriter* clone();

    virtual QString save(FileSignalReader& file_signal_reader,
                      SignalEventVector& event_vector,
                      const QString& file_name,
                      bool save_signals = true);
    
private:
    void updateEventTable (HDRTYPE* header, SignalEventVector& event_vector, double event_sample_rate);
    
    FileFormat target_type_;
    std::set<FileFormat> file_formats_support_event_saving_;
    mutable QMutex mutex_;
};

}

#endif /*BIOIGWRITER_H_*/
