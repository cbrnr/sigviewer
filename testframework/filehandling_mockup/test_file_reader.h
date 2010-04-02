#ifndef TEST_FILE_READER_H
#define TEST_FILE_READER_H

#include "../src/base/file_signal_reader.h"

class TestFileReader : public BioSig_::FileSignalReader
{
public:
    //-------------------------------------------------------------------------
    TestFileReader();

    //-------------------------------------------------------------------------
    virtual ~TestFileReader();

    //-------------------------------------------------------------------------
    virtual FileSignalReader* clone();

    //-------------------------------------------------------------------------
    virtual QString open(const QString& file_name);

    //-------------------------------------------------------------------------
    virtual QString open(const QString& file_name, const bool overflow_detection);

    //-------------------------------------------------------------------------
    virtual void enableCaching();

    //-------------------------------------------------------------------------
    virtual bool isOpen();

    //-------------------------------------------------------------------------
    virtual void close();

    //-------------------------------------------------------------------------
    virtual void loadSignals(SignalDataBlockPtrIterator begin,
                             SignalDataBlockPtrIterator end,
                             uint32 start_record);

    //-------------------------------------------------------------------------
    virtual void loadEvents(SignalEventVector& event_vector);

    //-------------------------------------------------------------------------
    virtual QPointer<BasicHeader> getBasicHeader ();

    //-------------------------------------------------------------------------
    virtual HDRTYPE* getRawHeader ();

private:
    //-------------------------------------------------------------------------
    TestFileReader (const& TestFileReader);
    const TestFileReader& operator= (const& TestFileReader);
};

#endif // TEST_FILE_READER_H
