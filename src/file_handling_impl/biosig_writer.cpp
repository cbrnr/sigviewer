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
#include "file_handler_factory_registrator.h"

#include <QFile>
#include <QMutexLocker>
#include <QMessageBox>
#include <QDebug>

namespace SigViewer_
{

FILE_SIGNAL_WRITER_REGISTRATION(gdf, BioSigWriter);

//-----------------------------------------------------------------------------
BioSigWriter::BioSigWriter ()
 : target_type_ (GDF)
{
    file_formats_support_event_saving_.insert(GDF1);
    file_formats_support_event_saving_.insert(GDF);
}

//-----------------------------------------------------------------------------
BioSigWriter::BioSigWriter (FileFormat target_type, QString new_file_path) :
        target_type_ (target_type),
        new_file_path_ (new_file_path)
{
    // nothing to do here
}


//-------------------------------------------------------------------------
QPair<FileSignalWriter*, QString> BioSigWriter::createInstance (QString const& file_path)
{
    BioSigWriter* writer (new BioSigWriter);
    writer->new_file_path_ = file_path;
    return QPair<FileSignalWriter*, QString> (writer, "");
}

//-----------------------------------------------------------------------------
bool BioSigWriter::supportsSavingEvents () const
{
    return file_formats_support_event_saving_.count(target_type_) > 0;
}

//-----------------------------------------------------------------------------
QString BioSigWriter::saveEventsToSignalFile (QSharedPointer<EventManager const> event_manager,
                                              std::set<EventType> const& types)
{
    if (file_formats_support_event_saving_.count(target_type_) == 0)
        return QObject::tr("Can't write events to that file that file type!");


    QList<EventID> events;
    foreach (EventType type, types)
        events.append(event_manager->getEvents(type));

    unsigned number_events = events.size();
    qDebug () << "number_events = " <<number_events;
    qDebug () << "event_manager->getNumberOfEvents() = " << event_manager->getNumberOfEvents();
    HDRTYPE* header = constructHDR (0, number_events);

    qDebug () << "BioSigWriter::saveEventsToSignalFile to " << new_file_path_;
    header = sopen (new_file_path_.toStdString().c_str(), "r", header);
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

    int error = sflush_gdf_event_table (header);
    if (error)
        QMessageBox::critical(0, "Events not saved!!!", QString::number(error));

    sclose (header);
    destructHDR (header);

    return "";
}


//-----------------------------------------------------------------------------
QString BioSigWriter::save (QSharedPointer<FileContext const> file_context,
                            std::set<EventType> const& types)
{
    QString source_file_path = file_context->getFilePathAndName();
    HDRTYPE* read_header = sopen (source_file_path.toStdString().c_str(), "r", NULL);
    uint32 read_data_size = read_header->NS * read_header->NRec * read_header->SPR;
    biosig_data_type* read_data = new biosig_data_type[read_data_size];
    sread (read_data, 0, read_data_size, read_header);

    read_header->TYPE = target_type_;
    if (target_type_ == GDF)
        read_header->VERSION = 2;

    HDRTYPE* write_header = sopen (new_file_path_.toStdString().c_str(), "w", read_header);
    qDebug() << "write NELEM = " << swrite (read_data, read_header->NRec, write_header);

    delete read_data;

    sclose (write_header);
    sclose (read_header);

    destructHDR (write_header);

    if (file_formats_support_event_saving_.count(target_type_))
        saveEventsToSignalFile (file_context->getEventManager (), types);

    return "";
}

}
