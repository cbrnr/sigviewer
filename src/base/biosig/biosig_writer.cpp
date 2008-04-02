#include "biosig_writer.h"

#include <QFile>

#include <iostream>

namespace BioSig_
{

BioSigWriter::BioSigWriter(FileFormat target_type)
 : target_type_ (target_type)
{
    file_formats_without_event_saving_.insert(BKR);
}

BioSigWriter::~BioSigWriter()
{
}

FileSignalWriter* BioSigWriter::clone()
{
    return new BioSigWriter (target_type_);
}

QString BioSigWriter::save(FileSignalReader& file_signal_reader,
                  SignalEventVector& event_vector,
                  const QString& file_name,
                  bool save_signals)
{
    if (file_formats_without_event_saving_.count(target_type_))
        return "events can't be saved in that file format, please export events";
    
    HDRTYPE* header = file_signal_reader.getRawHeader();
    
    if (header == 0)
        return "no biosig header found";
    
    QString save_file_name = file_name + ".tmp";
     
    if (target_type_ != header->TYPE)
        return "conversion of file formats not supported at the moment";
    
    
    
    QFile original_file (file_signal_reader.getBasicHeader()->getFullFileName());
    original_file.copy (save_file_name);
    
    HDRTYPE* new_header = sopen (save_file_name.toLocal8Bit ().data(), "wb", 0);
    
    biosig_data_type* data = header->data.block;
    sread(NULL, 0, header->NRec, header);
   
    std::cout << save_file_name.toLocal8Bit ().data() << std::endl;
    
    header = sopen(save_file_name.toLocal8Bit ().data(), "wb", header);
    swrite (data, header->NRec, header);
    sclose (header);
    
    QFile file (file_name);
    if (save_file_name != file_name)
    {
        file_signal_reader.close();
        file.setFileName(file_name);
        file.remove();
        file.setFileName(save_file_name);
        file.rename(file_name);
        file_signal_reader.open(file_name);
    }
    
    return "";
}


}
