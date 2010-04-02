// file_signal_writer.cpp

#include "file_signal_writer.h"

#include <QTextStream>

namespace BioSig_
{

// constructor
FileSignalWriter::FileSignalWriter()
: log_stream_(0)
{
    // nothing
}

// destructor
FileSignalWriter::~FileSignalWriter()
{
    // nothing
}

// set log stream
void FileSignalWriter::setLogStream(QTextStream* log_stream)
{
    log_stream_ = log_stream;
}

} // namespace BioSig_
