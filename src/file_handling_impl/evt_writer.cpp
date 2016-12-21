// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "evt_writer.h"
#include "file_handling/file_signal_writer_factory.h"

#include <QDebug>

#include <biosig.h>

namespace sigviewer
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

    qDebug () << "write events to " << new_file_path_;

    HDRTYPE *new_header = sopen (new_file_path_.toLocal8Bit().data(), "w", header);
    sclose (new_header);
    destructHDR (new_header);

    return "";
}

}
