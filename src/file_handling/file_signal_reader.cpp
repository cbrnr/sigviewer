// file_signal_reader.cpp

#include "file_signal_reader.h"

#include <QTextStream>
#include <QString>

namespace BioSig_
{

// constructor
FileSignalReader::FileSignalReader()
: log_stream_(0)
{
    // nothing
}

// destructor
FileSignalReader::~FileSignalReader()
{
    // nothing
}

// set log stream
void FileSignalReader::setLogStream(QTextStream* log_stream)
{
    log_stream_ = log_stream;
}

} // namespace BioSig_
