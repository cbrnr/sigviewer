#ifndef SINUS_DUMMY_READER_H
#define SINUS_DUMMY_READER_H

#include "file_handling/file_signal_reader.h"
#include "base/data_block.h"

#include <QMap>

namespace SigViewer_
{

class SinusDummyReader : public FileSignalReader
{
public:
    SinusDummyReader ();

    virtual ~SinusDummyReader();

    //-------------------------------------------------------------------------
    virtual QPair<FileSignalReader*, QString> createInstance (QString const& file_path);

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID channel_id,
                                                           unsigned start_sample,
                                                           unsigned length) const;

    //-------------------------------------------------------------------------
    virtual QList<QSharedPointer<SignalEvent const> > getEvents () const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader> getBasicHeader ();

    //-------------------------------------------------------------------------
    virtual QSharedPointer<BasicHeader const> getBasicHeader () const {return header_;}
private:
    QMap<ChannelID, QSharedPointer<DataBlock const> > data_;
    QList<QSharedPointer<SignalEvent const> > events_;
    QSharedPointer<BasicHeader> header_;
};

} // namespace SigViewer_

#endif // SINUS_DUMMY_READER_H
