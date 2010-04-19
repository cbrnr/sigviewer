#ifndef DUMMY_FILE_SIGNAL_READER_H
#define DUMMY_FILE_SIGNAL_READER_H

#include "dummy_basic_header.h"
#include "file_handling/file_signal_reader.h"


#include <cmath>

using BioSig_::DataBlock;
using BioSig_::BasicHeader;

class DummyFileSignalReader : public BioSig_::FileSignalReader
{
public:
    DummyFileSignalReader () {open_ = false;}
    virtual ~DummyFileSignalReader () {}
    virtual FileSignalReader* clone ()
    {
        return new DummyFileSignalReader ();
    }

    virtual QString open(const QString&)
    {
        open_ = true;
        return "";
    }
    virtual QString open(const QString&, const bool)
    {
        open_ = true;
        return "";
    }

    virtual void enableCaching() {}

    virtual bool isOpen()
    { return open_; }
    virtual void close() {open_ = false; }

    virtual QSharedPointer<DataBlock const> getSignalData (ChannelID,
                                                           unsigned start_sample,
                                                           unsigned length) const
    {
        QSharedPointer<DataBlock> data_block (new DataBlock);
        std::vector<float32> data (length);
        for (unsigned index = 0; index < length; index++)
            data[index] = sin (start_sample + index);
        data_block->setData (data, 50);
        return data_block;
    }

    virtual void loadEvents(SignalEventVector&)
    {

    }

    virtual QPointer<BasicHeader> getBasicHeader ()
    {
        return QPointer<BasicHeader> (new DummyBasicHeader);
    }

private:
    bool open_;
};

#endif // DUMMY_FILE_SIGNAL_READER_H
