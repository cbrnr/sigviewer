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
    BioSigWriter (FileFormat target_type);
    BioSigWriter (bool prototype_instance);
    virtual ~BioSigWriter();

    virtual FileSignalWriter* clone();

    virtual QString newSave (QSharedPointer<ChannelManager> channel_manager,
                             QSharedPointer<EventManager> event_manager,
                             QString const& file_path);



    virtual QString save(FileSignalReader& file_signal_reader,
                      SignalEventVector& event_vector,
                      const QString& file_name,
                      bool save_signals = true);
    
private:
    static BioSigWriter prototype_instance_;
    void updateEventTable (HDRTYPE* header, SignalEventVector& event_vector, double event_sample_rate);
    
    FileFormat target_type_;
    std::set<FileFormat> file_formats_support_event_saving_;
    mutable QMutex mutex_;
};

}

#endif /*BIOIGWRITER_H_*/
