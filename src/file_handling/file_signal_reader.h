// © SigViewer developers
//
// License: GPL-3.0

#ifndef FILE_SIGNAL_READER_H
#define FILE_SIGNAL_READER_H

#include <QFile>
#include <QPair>
#include <QPointer>
#include <QSharedPointer>
#include <QVector>

#include "base/data_block.h"
#include "base/signal_event.h"
#include "basic_header.h"

class QString;

namespace sigviewer {

//-----------------------------------------------------------------------------
/// FileSignalReader
///
/// abstract base class for reading signals from a file
class FileSignalReader {
   public:
    typedef QVector<SignalEvent> SignalEventVector;

    virtual ~FileSignalReader() {}

    virtual QPair<FileSignalReader*, QString> createInstance(QString const& file_path) = 0;

    virtual QSharedPointer<DataBlock const> getSignalData(ChannelID channel_id,
        size_t start_sample,
        size_t length) const = 0;

    virtual QList<QSharedPointer<SignalEvent const> > getEvents() const = 0;

    virtual QSharedPointer<BasicHeader> getBasicHeader() = 0;

    virtual QSharedPointer<BasicHeader const> getBasicHeader() const = 0;

    int setEventTypeColors(); /*!< Set a distinct color for each event type. */

   protected:
    FileSignalReader() {}

   private:
    Q_DISABLE_COPY(FileSignalReader)
};

}  // namespace sigviewer

#endif
