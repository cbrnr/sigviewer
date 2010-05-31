#ifndef DUMMY_FILE_SIGNAL_READER_H
#define DUMMY_FILE_SIGNAL_READER_H

#include "dummy_basic_header.h"
#include "file_handling/file_signal_reader.h"


#include <cmath>

using namespace BioSig_;

class DummyFileSignalReader : public FileSignalReader
{
public:
    DummyFileSignalReader () {}
    virtual ~DummyFileSignalReader () {}
    virtual QSharedPointer<FileSignalReader> createInstance (QString const&)
    {
        return QSharedPointer<DummyFileSignalReader>(new DummyFileSignalReader);
    }

    virtual QList<QSharedPointer<SignalEvent const> > getEvents () const
    {
        QList<QSharedPointer<SignalEvent const> > events;
        return  events;
    }

    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID,
                                                           unsigned start_sample,
                                                           unsigned length) const
    {
        QSharedPointer<std::vector<float32> > data (new std::vector<float32>);
        for (unsigned index = 0; index < length; index++)
            data->push_back (sin (start_sample + index));
        QSharedPointer<DataBlock> data_block (new DataBlock (data, 50));
        return data_block;
    }

    virtual QSharedPointer<BasicHeader> getBasicHeader ()
    {
        return QSharedPointer<BasicHeader> (new DummyBasicHeader);
    }
};

#endif // DUMMY_FILE_SIGNAL_READER_H
