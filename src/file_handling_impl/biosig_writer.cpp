/*

    $Id: biosig_writer.cpp,v 1.7 2009/02/22 12:36:46 cle1109 Exp $
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


#include "biosig_writer.h"
#include "../file_handling/file_signal_writer_factory.h"

#include <QFile>
#include <QMutexLocker>

namespace BioSig_
{

BioSigWriter BioSigWriter::prototype_instance_ (true);


BioSigWriter::BioSigWriter(FileFormat target_type)
 : target_type_ (target_type)
{
    file_formats_support_event_saving_.insert(GDF1);
    file_formats_support_event_saving_.insert(GDF);
}

BioSigWriter::BioSigWriter (bool)
{
    FileSignalWriterFactory::getInstance()->addPrototype(".gdf", new BioSigWriter (GDF));
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
                  bool)
{
    QMutexLocker lock (&mutex_); 
    //return "not implemented yet"; 
    if (file_formats_support_event_saving_.count(target_type_) == 0)
        return "events can't be saved in that file format, please export events";

    HDRTYPE* header = file_signal_reader.getRawHeader();
    double event_sample_rate = file_signal_reader.getBasicHeader()->getEventSamplerate();

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

void BioSigWriter::updateEventTable (HDRTYPE* header, SignalEventVector& event_vector, double event_sample_rate)
{
    header->EVENT.SampleRate = event_sample_rate;

    /* realloc is faster than delete and new  
    
    if (header->EVENT.DUR)
        delete[] header->EVENT.DUR;
    if (header->EVENT.CHN)
        delete[] header->EVENT.CHN;
    if (header->EVENT.TYP)
        delete[] header->EVENT.TYP;
    if (header->EVENT.POS)
        delete[] header->EVENT.POS;

    
    header->EVENT.POS = new uint32_t [event_vector.size ()];
    header->EVENT.TYP = new uint16_t [event_vector.size ()];
    header->EVENT.DUR = new uint32_t [event_vector.size ()];
    header->EVENT.CHN = new uint16_t [event_vector.size ()];
    */

    header->EVENT.TYP = (typeof(header->EVENT.TYP)) realloc(header->EVENT.TYP,event_vector.size () * sizeof(typeof(*header->EVENT.TYP)));
    header->EVENT.POS = (typeof(header->EVENT.POS)) realloc(header->EVENT.POS,event_vector.size () * sizeof(typeof(*header->EVENT.POS)));
    header->EVENT.CHN = (typeof(header->EVENT.CHN)) realloc(header->EVENT.CHN,event_vector.size () * sizeof(typeof(*header->EVENT.CHN)));
    header->EVENT.DUR = (typeof(header->EVENT.DUR)) realloc(header->EVENT.DUR,event_vector.size () * sizeof(typeof(*header->EVENT.DUR)));

    header->EVENT.N = 0;
    for (SignalEventVector::iterator iter = event_vector.begin(); iter != event_vector.end(); ++iter, ++header->EVENT.N)
    {
        header->EVENT.POS[header->EVENT.N] = iter->getPosition ();
        header->EVENT.TYP[header->EVENT.N] = iter->getType ();
        header->EVENT.DUR[header->EVENT.N] = iter->getDuration ();
        if (iter->getChannel() != UNDEFINED_CHANNEL)
                header->EVENT.CHN[header->EVENT.N] = iter->getChannel () + 1;
        else
                header->EVENT.CHN[header->EVENT.N] = 0;
    }
}

}
