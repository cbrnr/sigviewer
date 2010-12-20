#ifndef FILE_SIGNAL_READER_H
#define FILE_SIGNAL_READER_H

#include "basic_header.h"
#include "base/signal_event.h"
#include "base/data_block.h"

#include <QVector>
#include <QPointer>
#include <QSharedPointer>
#include <QPair>

class QString;

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// FileSignalReader
///
/// abstract base class for reading signals from a file
class FileSignalReader
{
public:
    typedef QVector<SignalEvent> SignalEventVector;

    virtual ~FileSignalReader() {}

    virtual QPair<FileSignalReader*, QString> createInstance (QString const& file_path) = 0;

    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID channel_id,
                                                           unsigned start_sample,
                                                           unsigned length) const = 0;

    virtual QList<QSharedPointer<SignalEvent const> > getEvents () const = 0;

    virtual QSharedPointer<BasicHeader> getBasicHeader () = 0;

    virtual QSharedPointer<BasicHeader const> getBasicHeader () const = 0;

protected:
    FileSignalReader () {}

private:
    Q_DISABLE_COPY(FileSignalReader)
};

} // namespace SigViewer_

#endif
