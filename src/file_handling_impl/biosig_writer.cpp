// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "biosig_writer.h"
#include "file_handler_factory_registrator.h"


namespace sigviewer
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
        return QObject::tr("Cannot write events to this file type!");


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
    header->EVENT.TYP = (decltype(header->EVENT.TYP)) realloc(header->EVENT.TYP,number_events * sizeof(decltype(*header->EVENT.TYP)));
    header->EVENT.POS = (decltype(header->EVENT.POS)) realloc(header->EVENT.POS,number_events * sizeof(decltype(*header->EVENT.POS)));
    header->EVENT.CHN = (decltype(header->EVENT.CHN)) realloc(header->EVENT.CHN,number_events * sizeof(decltype(*header->EVENT.CHN)));
    header->EVENT.DUR = (decltype(header->EVENT.DUR)) realloc(header->EVENT.DUR,number_events * sizeof(decltype(*header->EVENT.DUR)));
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
        QMessageBox::critical(0, QObject::tr("Events not saved!!!"), QString::number(error));

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

    delete[] read_data;

    sclose(write_header);
    sclose(read_header);

    destructHDR (write_header);

    if (file_formats_support_event_saving_.count(target_type_))
        saveEventsToSignalFile (file_context->getEventManager (), types);

    return "";
}

}
