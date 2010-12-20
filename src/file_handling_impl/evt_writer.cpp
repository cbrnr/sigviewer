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
#include "file_handling/file_signal_writer_factory.h"

#include <QDebug>

#include <biosig.h>

namespace SigViewer_
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
    FileSignalWriterFactory::getInstance()->registerHandler("evt", QSharedPointer<EVTWriter>(new EVTWriter));
}

//-----------------------------------------------------------------------------
EVTWriter::EVTWriter (QString const& new_file_path) :
        new_file_path_ (new_file_path)
{
    // nothing to do here
}


//-----------------------------------------------------------------------------
EVTWriter::~EVTWriter()
{
    // nothing to do here
}


//-------------------------------------------------------------------------
QPair<FileSignalWriter*, QString> EVTWriter::createInstance (QString const& new_file_path)
{
  return QPair<FileSignalWriter*, QString> (new EVTWriter (new_file_path), "");
}

//-----------------------------------------------------------------------------
QString EVTWriter::save(QSharedPointer<FileContext const> file_context,
                        std::set<EventType> const& types)
{
    QSharedPointer<EventManager const> event_manager = file_context->getEventManager();
    QList<EventID> events;
    foreach (EventType type, types)
        events.append(event_manager->getEvents(type));
    unsigned number_events = events.size();

    HDRTYPE* header = constructHDR (0, number_events);
    header->TYPE = GDF;
    header->VERSION = 2.0;
    header->SampleRate = event_manager->getSampleRate();
    header->EVENT.SampleRate = event_manager->getSampleRate();
    header->EVENT.N = number_events;
    header->EVENT.TYP = (typeof(header->EVENT.TYP)) realloc(header->EVENT.TYP,number_events * sizeof(typeof(*header->EVENT.TYP)));
    header->EVENT.POS = (typeof(header->EVENT.POS)) realloc(header->EVENT.POS,number_events * sizeof(typeof(*header->EVENT.POS)));
    header->EVENT.CHN = (typeof(header->EVENT.CHN)) realloc(header->EVENT.CHN,number_events * sizeof(typeof(*header->EVENT.CHN)));
    header->EVENT.DUR = (typeof(header->EVENT.DUR)) realloc(header->EVENT.DUR,number_events * sizeof(typeof(*header->EVENT.DUR)));
    for (unsigned index = 0; index < number_events; index++)
    {
        QSharedPointer<SignalEvent const> event = event_manager->getEvent(events[index]);
        if (event->getChannel() == UNDEFINED_CHANNEL)
            header->EVENT.CHN[index] = 0;
        else
            header->EVENT.CHN[index] = event->getChannel() + 1;
        header->EVENT.TYP[index] = event->getType ();
        header->EVENT.POS[index] = event->getPosition();
        header->EVENT.DUR[index] = event->getDuration();
    }

//    int error = sflush_gdf_event_table (header);
//    if (error)
//        QMessageBox::critical(0, "Events not saved!!!", QString::number(error));


    qDebug () << "write events to " << new_file_path_;

    HDRTYPE *new_header = sopen (new_file_path_.toLocal8Bit().data(), "w", header);
    sclose (new_header);
    destructHDR (new_header);

    return "";
//
//    if (file_name == file_signal_reader.getBasicHeader()->getFullFileName())
//        return "cannot remove signal data from open file";
//
//    HDRTYPE *header = constructHDR(0, event_vector.size ());
//    header->TYPE = GDF;
//                            header->VERSION = 2.0;
//
//                            header->EVENT.SampleRate = file_signal_reader.getBasicHeader()->getEventSamplerate();
//                            HDRTYPE *old_header = file_signal_reader.getRawHeader();
//                            header->SPR = old_header->SPR;
//                            header->SampleRate = old_header->SampleRate;
//
//                            /* (C) 2008 AS: keep patient information */
//                            header->T0             = old_header->T0;
//                            strcpy(header->Patient.Id, old_header->Patient.Id);
//                            header->Patient.Sex     = old_header->Patient.Sex;
//                            header->Patient.Weight     = old_header->Patient.Weight;
//                            header->Patient.Height     = old_header->Patient.Height;
//                            header->Patient.Birthday     = old_header->Patient.Birthday;
//                            header->Patient.Handedness     = old_header->Patient.Handedness;
//                            header->Patient.Smoking     = old_header->Patient.Smoking;
//                            header->Patient.AlcoholAbuse= old_header->Patient.AlcoholAbuse;
//                            header->Patient.DrugAbuse     = old_header->Patient.DrugAbuse;
//                            header->Patient.Medication     = old_header->Patient.Medication;
//                            header->Patient.Impairment.Visual = old_header->Patient.Impairment.Visual;
//
//                            /* (C) 2008 AS: managing Event table simplified */
//                            uint32 event_nr = 0;
//                            for (SignalEventVector::iterator iter = event_vector.begin(); iter != event_vector.end(); ++iter, event_nr++)
//                            {
//                              header->EVENT.POS[event_nr] = iter->getPosition ();
//                              header->EVENT.TYP[event_nr] = iter->getType ();
//                              header->EVENT.DUR[event_nr] = iter->getDuration ();
//                              if (iter->getChannel() != UNDEFINED_CHANNEL)
//                                  header->EVENT.CHN[event_nr] = iter->getChannel () + 1;
//                              else
//                                  header->EVENT.CHN[event_nr] = 0;
//                            }
//                            header->EVENT.N = event_nr;
//                            header->EVENT.SampleRate = header->SampleRate;
//
//                            HDRTYPE *new_header = sopen (file_name.toLocal8Bit ().data(), "w", header);
//                            sclose (new_header);
////                            new_header = sopen (file_name.toLocal8Bit ().data(), "r", 0);
//
////                             new_header->EVENT.N = event_vector.size ();
////                             new_header->EVENT.SampleRate = file_signal_reader.getBasicHeader()->getEventSamplerate();
////                             bool dur_was_set = false;
////                             bool chn_was_set = false;
////                             if (old_header->EVENT.DUR && new_header->EVENT.DUR)
////                               dur_was_set = true;
////                             else if (old_header->EVENT.DUR && !(new_header->EVENT.DUR))
////                             {
////                               dur_was_set = true;
////                               new_header->EVENT.DUR = new uint32_t [event_vector.size ()];
////                             }
////                             else if (!(old_header->EVENT.DUR) && new_header->EVENT.DUR)
////                               delete[] new_header->EVENT.DUR;
////
////                             if (old_header->EVENT.CHN && new_header->EVENT.CHN)
////                               chn_was_set = true;
////                             else if (old_header->EVENT.CHN && !(new_header->EVENT.CHN))
////                             {
////                               chn_was_set = true;
////                               new_header->EVENT.CHN = new uint16_t [event_vector.size ()];
////                             }
////                             else if (!(old_header->EVENT.CHN) && new_header->EVENT.CHN)
////                               delete[] new_header->EVENT.CHN;
////
////
////
////                             uint32 event_nr = 0;
////                             for (SignalEventVector::iterator iter = event_vector.begin(); iter != event_vector.end(); ++iter, event_nr++)
////                             {
////                               new_header->EVENT.POS[event_nr] = iter->getPosition ();
////                               new_header->EVENT.TYP[event_nr] = iter->getType ();
////                               if (dur_was_set)
////                                 new_header->EVENT.DUR[event_nr] = iter->getDuration ();
////                               if (chn_was_set)
////                               {
////                                 if (iter->getChannel() >= -1)
////                                   new_header->EVENT.CHN[event_nr] = iter->getChannel () + 1;
////                                 else
////                                   new_header->EVENT.CHN[event_nr] = 0;
////                               }
////                             }
////
////                             if (sflush_gdf_event_table(new_header))
////                               return "save event table failed";
//
//    return "";
}


} // namespace SigViewer_
