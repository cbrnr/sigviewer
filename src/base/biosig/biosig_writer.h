#ifndef BIOIGWRITER_H_
#define BIOIGWRITER_H_

#include "../file_signal_writer.h"

#include <biosig.h>

#include <QString>
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
    FileFormat target_type_;
    std::set<FileFormat> file_formats_without_event_saving_;
};

}

#endif /*BIOIGWRITER_H_*/
