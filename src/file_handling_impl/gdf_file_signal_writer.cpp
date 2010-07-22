#include "gdf_file_signal_writer.h"
#include "file_handling/file_signal_writer_factory.h"
#include "file_handler_factory_registrator.h"
#include "gui/progress_bar.h"

#include "biosig.h"

#include <libgdf/gdfwriter.h>

#include <QDebug>

namespace BioSig_
{

FILE_SIGNAL_WRITER_REGISTRATION(gdf, GDFFileSignalWriter);

//-------------------------------------------------------------------------
GDFFileSignalWriter::GDFFileSignalWriter()
{
    qDebug() << "GDFFileSignalWriter constructed!";
}

//-------------------------------------------------------------------------
GDFFileSignalWriter::GDFFileSignalWriter (QString const& file_path)
    : new_file_path_ (file_path)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
QSharedPointer<FileSignalWriter> GDFFileSignalWriter::createInstance (QString const& file_path)
{
    return QSharedPointer<FileSignalWriter> (new GDFFileSignalWriter (file_path));
}


//-------------------------------------------------------------------------
QString GDFFileSignalWriter::saveEventsToSignalFile (QSharedPointer<EventManager const> event_manager,
                                        std::set<EventType> const& types)
{
    // FIXXXME: REMOVE AS SOON AS LIBGDF SUPPORTS SAVING EVENTS TO EXISTING FILES
    QList<EventID> events;
    foreach (EventType type, types)
        events.append(event_manager->getEvents(type));

    unsigned number_events = events.size();
    qDebug () << "number_events = " <<number_events;
    qDebug () << "event_manager->getNumberOfEvents() = " << event_manager->getNumberOfEvents();
    HDRTYPE* header = constructHDR (0, number_events);

    qDebug () << "GDFFileSignalWriter::saveEventsToSignalFile to " << new_file_path_;
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
        header->EVENT.DUR[index] = event->getDuration();
        header->EVENT.TYP[index] = event->getType ();
        header->EVENT.POS[index] = event->getPosition();
    }

    int error = sflush_gdf_event_table (header);

    sclose (header);
    destructHDR (header);
    if (error)
        return "Events not saved!!!! (biosig error)";
    else
        return "";
}

//-------------------------------------------------------------------------
QString GDFFileSignalWriter::save (QSharedPointer<FileContext const> file_context,
                                   std::set<EventType> const&)
{
    qDebug () << "GDFFileSignalWriter::save";
    GDFWriter writer (new_file_path_.toStdString());

    QSharedPointer<ChannelManager const> channel_manager = file_context->getChannelManager();
    writer.addSignal (16, channel_manager->getSampleRate(), channel_manager->getNumberChannels());
    writer.forceRecordDuration (true, channel_manager->getNumberSamples(), channel_manager->getSampleRate());

    writer.createHeader ();

    writer.setEventMode (3, channel_manager->getSampleRate());
    for (unsigned channel_index = 0;
         channel_index < channel_manager->getNumberChannels();
         channel_index++)
    {
        writer.getSignalHeader()->physmin[channel_index] = channel_manager->getMinValue (channel_index);
        writer.getSignalHeader()->physmax[channel_index] = channel_manager->getMaxValue (channel_index);
        writer.getSignalHeader()->digmin[channel_index] = channel_manager->getMinValue (channel_index);
        writer.getSignalHeader()->digmax[channel_index] = channel_manager->getMaxValue (channel_index);
        QString label = channel_manager->getChannelLabel (channel_index);
        for (int label_character_index = 0; label_character_index < 16 && label_character_index < label.size(); label_character_index++)
            writer.getSignalHeader()->label[channel_index][label_character_index] = label.at(label_character_index).toAscii();
    }

    try
    {
        writer.open ();
    }
    catch (StrException& exc)
    {
        qDebug () << "EXCEPTION OPEN: " << exc.msg.c_str();
        return exc.msg.c_str();
    }

    try
    {
        QString progressbar_string = "Writing samples...";
        for (unsigned sample_index = 0; sample_index < channel_manager->getNumberSamples(); sample_index++)
        {
            foreach (ChannelID channel_id, channel_manager->getChannels())
            {
                writer.addSample (channel_id, (*(channel_manager->getData (channel_id, sample_index, 1)))[0]);
            }
            ProgressBar::instance().increaseValue (1, progressbar_string);
        }
        progressbar_string = "Writing events...";
        QSharedPointer<EventManager const> event_manager = file_context->getEventManager();
        if (!event_manager.isNull())
        {
            foreach (EventID event_id, event_manager->getAllEvents())
            {
                QSharedPointer<SignalEvent const> event = event_manager->getEvent(event_id);
                writer.addEvent (event->getPosition()+1, event->getType(), event->getChannel()+1, event->getDuration());
                ProgressBar::instance().increaseValue (1, progressbar_string);
            }
        }
    }
    catch (StrException& exc)
    {
        qDebug () << "EXCEPTION ADD SAMPLE: " << exc.msg.c_str();
        return exc.msg.c_str();
    }

    try
    {
        writer.close ();
    }
    catch (StrException& exc)
    {
        qDebug () << "EXCEPTION CLOSE: " << exc.msg.c_str();
        return exc.msg.c_str();
    }

    return "";
}

}
