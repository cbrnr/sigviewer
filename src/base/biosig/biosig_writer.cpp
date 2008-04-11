#include "biosig_writer.h"

#include <QFile>
#include <QMutexLocker>

#include <iostream>

namespace BioSig_
{

BioSigWriter::BioSigWriter(FileFormat target_type)
 : target_type_ (target_type)
{
    file_formats_support_event_saving_.insert(GDF1);
    file_formats_support_event_saving_.insert(GDF);

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
    QMutexLocker lock (&mutex_); 
    //return "not implemented yet"; 
    if (file_formats_support_event_saving_.count(target_type_) == 0)
        return "events can't be saved in that file format, please export events";
    
    HDRTYPE* header = file_signal_reader.getRawHeader();
    uint32 event_sample_rate = file_signal_reader.getBasicHeader()->getEventSamplerate();
    
    if (header == 0)
        return "no biosig header found";
    
    QString save_file_name = file_name + ".tmp";
    QString original_file_name =  file_signal_reader.getBasicHeader()->getFullFileName();
    
    if (original_file_name == file_name)
    {
        updateEventTable (header, event_vector, event_sample_rate);
        
        if (sflush_gdf_event_table(header))
            return "save event table failed";
    }
    else
    {
    
        if (target_type_ != header->TYPE)
            return "conversion of file formats not supported at the moment";
    
        QFile original_file (original_file_name);
        original_file.copy (save_file_name);
    
        HDRTYPE* new_header = sopen (save_file_name.toLocal8Bit ().data(), "r", NULL);
    
        updateEventTable (new_header, event_vector, event_sample_rate);
        
        if (sflush_gdf_event_table(new_header))
        {
            sclose (new_header); 
            return "save event table failed";
        }
        
        sclose (new_header);
        
        QFile saved_file (save_file_name);
        saved_file.rename (file_name);
        saved_file.close();
    }
    
    
    return "";
}

void BioSigWriter::updateEventTable (HDRTYPE* header, SignalEventVector& event_vector, uint32 event_sample_rate)
{
    if (header->EVENT.N != static_cast<uint32_t>(event_vector.size ()))
    {
        header->EVENT.N = event_vector.size ();
        header->EVENT.SampleRate = event_sample_rate;
        bool dur_was_set = false;
        bool chn_was_set = false;
        if (header->EVENT.DUR)
        {
            delete[] header->EVENT.DUR;
            dur_was_set = true;
        }
        if (header->EVENT.CHN)
        {
            delete[] header->EVENT.CHN;
            chn_was_set = true;
        }
        if (header->EVENT.TYP)
            delete[] header->EVENT.TYP;
        if (header->EVENT.POS)
            delete[] header->EVENT.POS;

        
        header->EVENT.POS = new uint32_t [event_vector.size ()];
        header->EVENT.TYP = new uint16_t [event_vector.size ()];
        if (dur_was_set)
            header->EVENT.DUR = new uint32_t [event_vector.size ()];
        if (chn_was_set)
            header->EVENT.CHN = new uint16_t [event_vector.size ()];
            
        uint32 event_nr = 0;
        for (SignalEventVector::iterator iter = event_vector.begin(); iter != event_vector.end(); ++iter, event_nr++)
        {
            header->EVENT.POS[event_nr] = iter->getPosition ();
            header->EVENT.TYP[event_nr] = iter->getType ();
            if (dur_was_set)
                header->EVENT.DUR[event_nr] = iter->getDuration ();
            if (chn_was_set)
            {
                if (iter->getChannel() >= -1)
                {
                    header->EVENT.CHN[event_nr] = iter->getChannel () + 1;
                    std::cout << "CHANNEL SET TO " << header->EVENT.CHN[event_nr] << std::cout;
                }
                else
                    header->EVENT.CHN[event_nr] = 0;
            }
        }

    } 

}


}
