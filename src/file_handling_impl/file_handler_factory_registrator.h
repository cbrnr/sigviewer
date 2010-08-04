#ifndef FILE_HANDLER_FACTORY_REGISTRATOR_H
#define FILE_HANDLER_FACTORY_REGISTRATOR_H

#include "file_handling/file_signal_writer_factory.h"
#include "file_handling/file_signal_reader_factory.h"

namespace SigViewer_
{

#define FILE_SIGNAL_WRITER_REGISTRATION(file_ending, writer) \
    namespace Registrators_ { \
    FileSignalWriterFactoryRegistrator file_writer_registrator ## writer ## file_ending (#file_ending, QSharedPointer<FileSignalWriter> (new writer)); \
    }

#define FILE_SIGNAL_READER_REGISTRATION(file_ending, reader) \
    namespace Registrators_ { \
    FileSignalReaderFactoryRegistrator file_reader_registrator ## reader ## file_ending (#file_ending, QSharedPointer<FileSignalReader> (new reader)); \
    }

#define FILE_SIGNAL_READER_DEFAULT_REGISTRATION(reader) \
    namespace Registrators_ { \
    FileSignalReaderFactoryRegistrator file_reader_registrator ## reader (QSharedPointer<FileSignalReader> (new reader)); \
    }


namespace Registrators_
{

//-----------------------------------------------------------------------------
class FileSignalWriterFactoryRegistrator
{
public:
    FileSignalWriterFactoryRegistrator (QString const& file_ending,
                                        QSharedPointer<FileSignalWriter> writer)
    {
        FileSignalWriterFactory::getInstance()->registerHandler (file_ending, writer);
    }
};

//-----------------------------------------------------------------------------
class FileSignalReaderFactoryRegistrator
{
public:
    FileSignalReaderFactoryRegistrator (QString const& file_ending,
                                        QSharedPointer<FileSignalReader> reader)
    {
        FileSignalReaderFactory::getInstance()->registerHandler (file_ending, reader);
    }
    FileSignalReaderFactoryRegistrator (QSharedPointer<FileSignalReader> reader)
    {
        FileSignalReaderFactory::getInstance()->registerDefaultHandler (reader);
    }
};

} // namespace Registrators_

} // namespace SigViewer_

#endif // FILE_HANDLER_FACTORY_REGISTRATOR_H
