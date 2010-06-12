#include "test.h"
#include "file_handling_impl/event_manager_impl.h"
#include "file_handling/file_signal_reader_factory.h"



namespace BioSig_
{

namespace Tests_
{

QSharedPointer<EventManager> Test::createEventManagerWithDummyData ()
{
    QSharedPointer<FileSignalReader> reader = FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy");
    return QSharedPointer<EventManager> (new EventManagerImpl(reader));
}

}

}
