// © SigViewer developers
//
// License: GPL-3.0

#ifndef EVENT_CSV_EXPORTER_H
#define EVENT_CSV_EXPORTER_H

#include <QString>

namespace sigviewer
{

class EventManager;

/// Write all events to a CSV file ordered by sample position.
/// Returns false when the destination cannot be opened or written.
bool writeEventsToCSV (EventManager const& event_manager, QString const& file_path);

}

#endif // EVENT_CSV_EXPORTER_H
