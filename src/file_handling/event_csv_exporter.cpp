// © SigViewer developers
//
// License: GPL-3.0

#include "event_csv_exporter.h"

#include "event_manager.h"

#include <QVector>

#include <algorithm>
#include <fstream>

namespace sigviewer
{

bool writeEventsToCSV (EventManager const& event_manager, QString const& file_path)
{
    std::ofstream file(file_path.toStdString());
    if (!file.is_open())
        return false;

    struct Row {
        size_t position;
        size_t duration;
        int channel;
        int type;
        QString name;
    };

    QVector<Row> events;
    for (EventID event_id : event_manager.getAllEvents())
    {
        QSharedPointer<SignalEvent const> event = event_manager.getEvent(event_id);
        if (!event.isNull())
        {
            events.append({
                event->getPosition(),
                event->getDuration(),
                event->getChannel(),
                event->getType(),
                event_manager.getNameOfEvent(event_id)
            });
        }
    }

    std::sort(events.begin(), events.end(),
              [](Row const& left, Row const& right) {
                  return left.position < right.position;
              });

    file << "position,duration,channel,type,name\n";
    for (Row const& event : events)
    {
        QString name = event.name;
        name.remove(',');
        file << event.position << ',' << event.duration << ',' << event.channel << ','
             << event.type << ',' << name.toStdString() << '\n';
    }

    file.close();
    return !file.fail();
}

}
