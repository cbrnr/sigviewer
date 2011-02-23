#include "gdf_file_signal_writer.h"
#include "file_handling/file_signal_writer_factory.h"
#include "../file_handler_factory_registrator.h"
#include "gui/progress_bar.h"

#include "biosig.h"

#include <GDF/Writer.h>
#include <GDF/Modifier.h>

#include <QDebug>
#include <QMessageBox>

namespace SigViewer_
{

//FILE_SIGNAL_WRITER_REGISTRATION(gdf, GDFFileSignalWriter);

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
QPair<FileSignalWriter*, QString> GDFFileSignalWriter::createInstance (QString const& file_path)
{
    return QPair<FileSignalWriter*, QString> (new GDFFileSignalWriter (file_path), "");
}


//-------------------------------------------------------------------------
QString GDFFileSignalWriter::saveEventsToSignalFile (QSharedPointer<EventManager const> event_manager,
                                        std::set<EventType> const& types)
{
    gdf::Modifier modifier;
    modifier.open (new_file_path_.toStdString());
    gdf::EventHeader* event_header = modifier.getEventHeader();
    float32 event_sampling_rate = event_header->getSamplingRate();
    event_header->clear();
    event_header->setMode (3);
    event_header->setSamplingRate (event_sampling_rate);

    QList<EventID> events;
    foreach (EventType type, types)
        events.append(event_manager->getEvents(type));

    for (int index = 0; index < events.size(); index++)
    {
        gdf::Mode3Event gdf_event;
        QSharedPointer<SignalEvent const> event = event_manager->getEvent(events[index]);
        if (event->getChannel() == UNDEFINED_CHANNEL)
            gdf_event.channel = 0;
        else
            gdf_event.channel = event->getChannel() + 1;

        gdf_event.duration = event->getDurationInSec() * event_sampling_rate;
        gdf_event.type = event->getType();
        gdf_event.position = event->getPositionInSec() * event_sampling_rate;

        event_header->addEvent (gdf_event);
    }

    event_header->sort ();
    modifier.saveChanges ();
    modifier.close ();

    return "";
}

//-------------------------------------------------------------------------
QString GDFFileSignalWriter::save (QSharedPointer<FileContext const> file_context,
                                   std::set<EventType> const&)
{
    qDebug () << "GDFFileSignalWriter::save";
    gdf::Writer writer;
    writer.getMainHeader ().set_recording_id (file_context->getFileName().append(" converted to GDF2").toStdString());
    // GDFWriter writer (new_file_path_.toStdString());

    ChannelManager const& channel_manager = file_context->getChannelManager();

    foreach (ChannelID channel, channel_manager.getChannels())
    {
        writer.createSignal (channel);
        writer.getSignalHeader(channel).set_label (channel_manager.getChannelLabel (channel).toStdString());
        writer.getSignalHeader(channel).set_datatype (gdf::FLOAT64);
        writer.getSignalHeader(channel).set_samplerate (channel_manager.getSampleRate());
        writer.getSignalHeader(channel).set_digmin (-1);
        writer.getSignalHeader(channel).set_digmax (1);
        writer.getSignalHeader(channel).set_physmin (channel_manager.getMinValue (channel));
        writer.getSignalHeader(channel).set_physmax (channel_manager.getMaxValue (channel));
    }

    try
    {
        writer.setEventMode (3);
        writer.open (new_file_path_.toStdString(), gdf::writer_ev_memory | gdf::writer_overwrite);
    }
    catch (gdf::exception::header_issues &exception)
    {
        QString errors;
        if (exception.warnings.size () > 0)
        {
            foreach (std::string error, exception.errors)
                errors.append (error.c_str());

            QMessageBox::warning (0, "Warning", errors);
        }
        if (exception.errors.size () > 0)
        {
            foreach (std::string error, exception.errors)
                errors.append (error.c_str());

            return errors;
        }
    }

    try
    {
        QString progressbar_string = "Writing samples...";
        for (unsigned sample_index = 0; sample_index < channel_manager.getNumberSamples(); sample_index++)
        {
            foreach (ChannelID channel_id, channel_manager.getChannels())
            {
                writer.addSamplePhys (channel_id, (*(channel_manager.getData (channel_id, sample_index, 1)))[0]);
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
    catch (std::exception & exception)
    {
        return exception.what();
    }

    writer.close();

    return "";
}

}
