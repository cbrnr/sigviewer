/*
    $Id: evt_writer.cpp,v 1.6 2009/02/22 12:36:46 cle1109 Exp $
    Copyright (C) Thomas Brunner  2006,2007 
              Christoph Eibel 2007,2008, 
          Clemens Brunner 2006,2007,2008  
              Alois Schloegl  2008
    This file is part of the "SigViewer" repository 
    at http://biosig.sf.net/ 

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 
    
*/

#include "evt_writer.h"
#include "../file_handling/file_signal_writer_factory.h"
#include "../base/signal_data_block.h"

#include <biosig.h>
#include <iostream>

namespace BioSig_
{

//-----------------------------------------------------------------------------
EVTWriter EVTWriter::prototype_instance_ (true);

//-----------------------------------------------------------------------------
EVTWriter::EVTWriter()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
EVTWriter::EVTWriter (bool)
{
    FileSignalWriterFactory::getInstance()->addPrototype(".evt", new EVTWriter);
}

//-----------------------------------------------------------------------------
EVTWriter::~EVTWriter()
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
FileSignalWriter* EVTWriter::clone()
{
  FileSignalWriter* new_instance = new EVTWriter;
  new_instance->setLogStream(log_stream_);
  return new_instance;
}

//-----------------------------------------------------------------------------
QString EVTWriter::save(FileSignalReader& file_signal_reader,
                        SignalEventVector& event_vector,
                        const QString& file_name, bool)
{
    if (file_name == file_signal_reader.getBasicHeader()->getFullFileName())
        return "cannot remove signal data from open file";

    HDRTYPE *header = constructHDR(0, event_vector.size ());
    header->TYPE = GDF;
                            header->VERSION = 2.0;

                            header->EVENT.SampleRate = file_signal_reader.getBasicHeader()->getEventSamplerate();
                            HDRTYPE *old_header = file_signal_reader.getRawHeader();
                            header->SPR = old_header->SPR; 
                            header->SampleRate = old_header->SampleRate; 
                     
                            /* (C) 2008 AS: keep patient information */                            
                            header->T0             = old_header->T0; 
                            strcpy(header->Patient.Id, old_header->Patient.Id);
                            header->Patient.Sex     = old_header->Patient.Sex;
                            header->Patient.Weight     = old_header->Patient.Weight;
                            header->Patient.Height     = old_header->Patient.Height;
                            header->Patient.Birthday     = old_header->Patient.Birthday;
                            header->Patient.Handedness     = old_header->Patient.Handedness;
                            header->Patient.Smoking     = old_header->Patient.Smoking;
                            header->Patient.AlcoholAbuse= old_header->Patient.AlcoholAbuse;
                            header->Patient.DrugAbuse     = old_header->Patient.DrugAbuse;
                            header->Patient.Medication     = old_header->Patient.Medication;
                            header->Patient.Impairment.Visual = old_header->Patient.Impairment.Visual;
                            
                            /* (C) 2008 AS: managing Event table simplified */                            
                            uint32 event_nr = 0;
                            for (SignalEventVector::iterator iter = event_vector.begin(); iter != event_vector.end(); ++iter, event_nr++)
                            {
                              header->EVENT.POS[event_nr] = iter->getPosition ();
                              header->EVENT.TYP[event_nr] = iter->getType ();
                              header->EVENT.DUR[event_nr] = iter->getDuration ();
                              if (iter->getChannel() >= -1)
                                  header->EVENT.CHN[event_nr] = iter->getChannel () + 1;
                              else
                                  header->EVENT.CHN[event_nr] = 0;
                            } 
                            header->EVENT.N = event_nr; 
                            header->EVENT.SampleRate = header->SampleRate;

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
