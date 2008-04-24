// evt_writer.cpp

#include "evt_writer.h"
#include "../signal_data_block.h"
#include <biosig.h>
#include <iostream>

namespace BioSig_
{

// constructor
EVTWriter::EVTWriter()
{
  // nothing
}

// destructor
EVTWriter::~EVTWriter()
{
    // nothing
}

// clone
FileSignalWriter* EVTWriter::clone()
{
  FileSignalWriter* new_instance = new EVTWriter;
  new_instance->setLogStream(log_stream_);
  return new_instance;
}

// save
  QString EVTWriter::save(FileSignalReader& file_signal_reader,
                          SignalEventVector& event_vector,
                          const QString& file_name, bool save_signals)
                          {
                            if (file_name == file_signal_reader.getBasicHeader()->getFullFileName())
                                return "cannot remove signal data from open file";

                            HDRTYPE *header = constructHDR(0, event_vector.size ());
                            header->TYPE = GDF;
                            header->VERSION = 1.25f;
                            header->Dur[0] = 1;
                            header->Dur[1] = 1;
                            header->EVENT.SampleRate = file_signal_reader.getBasicHeader()->getEventSamplerate();
                            HDRTYPE *old_header = file_signal_reader.getRawHeader();
                            
                            bool dur_was_set = false;
                            bool chn_was_set = false;
                            if (old_header->EVENT.DUR && header->EVENT.DUR)
                              dur_was_set = true;
                            else if (old_header->EVENT.DUR && !(header->EVENT.DUR))
                            {
                              dur_was_set = true;
                              header->EVENT.DUR = new uint32_t [event_vector.size ()];
                            }
                            else if (!(old_header->EVENT.DUR) && header->EVENT.DUR)
                              delete[] header->EVENT.DUR;
                            
                            if (old_header->EVENT.CHN && header->EVENT.CHN)
                              chn_was_set = true;
                            else if (old_header->EVENT.CHN && !(header->EVENT.CHN))
                            {
                              chn_was_set = true;
                              header->EVENT.CHN = new uint16_t [event_vector.size ()];
                            }
                            else if (!(old_header->EVENT.CHN) && header->EVENT.CHN)
                              delete[] header->EVENT.CHN;

        
            
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
                                  header->EVENT.CHN[event_nr] = iter->getChannel () + 1;
                                else
                                  header->EVENT.CHN[event_nr] = 0;
                              }
                            } 
                            
                            HDRTYPE *new_header = sopen (file_name.toLocal8Bit ().data(), "w", header);
                            sclose (new_header);
//                            new_header = sopen (file_name.toLocal8Bit ().data(), "r", 0);

//                             new_header->EVENT.N = event_vector.size ();
//                             new_header->EVENT.SampleRate = file_signal_reader.getBasicHeader()->getEventSamplerate();
//                             bool dur_was_set = false;
//                             bool chn_was_set = false;
//                             if (old_header->EVENT.DUR && new_header->EVENT.DUR)
//                               dur_was_set = true;
//                             else if (old_header->EVENT.DUR && !(new_header->EVENT.DUR))
//                             {
//                               dur_was_set = true;
//                               new_header->EVENT.DUR = new uint32_t [event_vector.size ()];
//                             }
//                             else if (!(old_header->EVENT.DUR) && new_header->EVENT.DUR)
//                               delete[] new_header->EVENT.DUR;
//                             
//                             if (old_header->EVENT.CHN && new_header->EVENT.CHN)
//                               chn_was_set = true;
//                             else if (old_header->EVENT.CHN && !(new_header->EVENT.CHN))
//                             {
//                               chn_was_set = true;
//                               new_header->EVENT.CHN = new uint16_t [event_vector.size ()];
//                             }
//                             else if (!(old_header->EVENT.CHN) && new_header->EVENT.CHN)
//                               delete[] new_header->EVENT.CHN;
// 
//         
//             
//                             uint32 event_nr = 0;
//                             for (SignalEventVector::iterator iter = event_vector.begin(); iter != event_vector.end(); ++iter, event_nr++)
//                             {
//                               new_header->EVENT.POS[event_nr] = iter->getPosition ();
//                               new_header->EVENT.TYP[event_nr] = iter->getType ();
//                               if (dur_was_set)
//                                 new_header->EVENT.DUR[event_nr] = iter->getDuration ();
//                               if (chn_was_set)
//                               {
//                                 if (iter->getChannel() >= -1)
//                                   new_header->EVENT.CHN[event_nr] = iter->getChannel () + 1;
//                                 else
//                                   new_header->EVENT.CHN[event_nr] = 0;
//                               }
//                             } 
//                             
//                             if (sflush_gdf_event_table(new_header))
//                               return "save event table failed";

                            return "";
}


} // namespace BioSig_
