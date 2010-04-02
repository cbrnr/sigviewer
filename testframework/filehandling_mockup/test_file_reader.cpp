#include "test_file_reader.h"

//-------------------------------------------------------------------------
TestFileReader::TestFileReader ()
{

}

//-------------------------------------------------------------------------
TestFileReader::~TestFileReader()
{

}

//-------------------------------------------------------------------------
FileSignalReader* TestFileReader::clone()
{
    return 0;
}

//-------------------------------------------------------------------------
QString TestFileReader::open (const QString& file_name)
{
    return "";
}

//-------------------------------------------------------------------------
QString TestFileReader::open (const QString& file_name, const bool overflow_detection)
{
    return "";
}

//-------------------------------------------------------------------------
void TestFileReader::enableCaching ()
{

}

//-------------------------------------------------------------------------
bool TestFileReader::isOpen ()
{
    return false;
}

//-------------------------------------------------------------------------
void TestFileReader::close ()
{

}

//-------------------------------------------------------------------------
void TestFileReader::loadSignals(SignalDataBlockPtrIterator begin,
                         SignalDataBlockPtrIterator end,
                         uint32 start_record)
{

}

//-------------------------------------------------------------------------
void TestFileReader::loadEvents(SignalEventVector& event_vector)
{

}

//-------------------------------------------------------------------------
QPointer<BasicHeader> TestFileReader::getBasicHeader ()
{
    return QPointer<BasicHeader>(0);
}

//-------------------------------------------------------------------------
HDRTYPE* TestFileReader::getRawHeader ()
{
    return 0;
}
