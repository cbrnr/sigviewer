// gdf_reader.cpp

#include "gdf_reader.h"
#include "gdf_signal_header.h"
#include "../file_signal_reader_factory.h"
#include "../math_utils.h"
#include "../stream_utils.h"
#include "../signal_data_block.h"
#include "gdf_1_reader_impl.h"
#include "gdf_2_reader_impl.h"

#include <algorithm>

#include <QTextStream>


namespace BioSig_
{

// constructor
GDFReader::GDFReader()
: file_(new QFile()),
  basic_header_ (new BasicHeader ()),
  buffer_(0), 
  reader_impl_ (0)
{
    // nothing
}

// destructor
GDFReader::~GDFReader()
{
    if (file_->isOpen())
    {
        file_->close();
    }
    if (buffer_)
    {
        delete[] buffer_;
    }
}

// clone
FileSignalReader* GDFReader::clone()
{
    FileSignalReader* new_instance = new GDFReader;
    new_instance->setLogStream(log_stream_);
    return new_instance;
}

// check if open
bool GDFReader::isOpen()
{
    return file_->isOpen();
}

QPointer<BasicHeader> GDFReader::getBasicHeader ()
{
    return basic_header_;
}

// close
void GDFReader::close()
{
    if (file_->isOpen())
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::close '" << basic_header_->getFullFileName() << "'\n";
        }
        file_->close();
        basic_header_->resetBasicHeader();
    }
}

// open
bool GDFReader::open(const QString& file_name)
{
    qDebug( "GDFReader::open(const QString& file_name) 1" );
    if (file_->isOpen())
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::open '" << file_name << "' Error: '"
                        << basic_header_->getFullFileName() << "' not closed\n";
        }
        return false;
    }
    file_->setFileName(file_name);
    if (!file_->open(QIODevice::ReadOnly))
    {
        if (log_stream_)
        {
            *log_stream_ << "GDFReader::open '" << file_name
                        << "' Error: reading file\n";
        }
        return false;
    }

    // read headers
    if (log_stream_)
    {
        *log_stream_ << "GDFReader::open '" << file_name << "'\n";
    }
    char version_id [8];
    readStreamChars(version_id, *file_, sizeof(version_id));
    if (strncmp(version_id, "GDF 2.", 6) == 0 || strncmp(version_id, "GDF 1.9", 7) == 0)
    {
        reader_impl_.reset(new GDF2ReaderImpl (file_, basic_header_));
        basic_header_->setVersion(QString (version_id));
    }
    else
    if (strncmp(version_id, "GDF 1.", 6) == 0)
    {
        reader_impl_.reset(new GDF1ReaderImpl (file_, basic_header_));
        basic_header_->setVersion(QString (version_id));
    } else
    {
        file_->close ();
        basic_header_->resetBasicHeader();
        return false;
    }
    
    if (!reader_impl_->loadFixedHeader() ||
        !reader_impl_->loadSignalHeaders())
    {
        file_->close ();
        basic_header_->resetBasicHeader ();
        return false;
    }

    reader_impl_->loadEventTableHeader ();
//    if (!loadFixedHeader(file_name) ||
//        !loadSignalHeaders(file_name))
//    {
//        file_.close();
//        resetBasicHeader();
//        resetGDFHeader();
//        return false;
//    }
//    loadEventTableHeader();
    return true;
}


// load signals
void GDFReader::loadSignals(SignalDataBlockPtrIterator begin,
                            SignalDataBlockPtrIterator end,
                            uint32 start_record)
{
    reader_impl_->loadSignals(begin, end, start_record);
}

// load events
void GDFReader::loadEvents(SignalEventVector& event_vector)
{
    reader_impl_->loadEvents(event_vector);
}

// load raw records
bool GDFReader::loadRawRecords(float64** record_data, uint32 start_record,
                               uint32 records)
{
    return reader_impl_->loadRawRecords(record_data, start_record, records);
}

} // namespace BioSig_
