#include "file_handling_tests.h"
#include "gui_impl/commands/open_file_gui_command.h"
#include "file_handling/file_signal_writer_factory.h"
#include "file_handling/file_signal_reader_factory.h"

#include <QTemporaryFile>

namespace SigViewer_
{
namespace Tests_
{


//-----------------------------------------------------------------------------
void FileHandlingTests::init ()
{
    OpenFileGuiCommand::openFile ("blub.sinusdummy", true);
}

//-----------------------------------------------------------------------------
QString FileHandlingTests::run ()
{
    RUN_SUB_TEST (testEventExportingImporting())

    return "";
}

//-----------------------------------------------------------------------------
void FileHandlingTests::cleanup ()
{
    action("Close")->trigger();
}

//-----------------------------------------------------------------------------
QString FileHandlingTests::testEventExportingImporting ()
{
    QTemporaryFile event_file ("XXXXXX.evt");
    event_file.open();
    event_file.close();
    QSharedPointer<FileSignalWriter> writer (FileSignalWriterFactory::getInstance()->getHandler(event_file.fileName()));
    VERIFY (writer.isNull() == false, "EVT writer not created")
    QString error = writer->save (applicationContext()->getCurrentFileContext());
    VERIFY (error.size() == 0, error)

    QSharedPointer<FileSignalReader> reader (FileSignalReaderFactory::getInstance()->getHandler(event_file.fileName()));
    VERIFY (reader.isNull() == false, "EVT reader not created")
    QList<QSharedPointer<SignalEvent const> > events = reader->getEvents();
    QSharedPointer<EventManager> event_manager = applicationContext()->getCurrentFileContext()->getEventManager();
    VERIFY (static_cast<unsigned>(events.size()) == event_manager->getNumberOfEvents(), "not all events in file")


    QTemporaryFile event_file_2 ("XXXXXX.evt");
    event_file_2.open();
    event_file_2.close();
    QSharedPointer<FileSignalWriter> writer_2 (FileSignalWriterFactory::getInstance()->getHandler(event_file_2.fileName()));
    VERIFY (writer_2.isNull() == false, "EVT writer 2 not created")
    std::set<EventType> exported_event_types;
    exported_event_types.insert (event_manager->getEvent(0)->getType());
    error = writer_2->save (applicationContext()->getCurrentFileContext(), exported_event_types);
    VERIFY (error.size() == 0, error)
    QSharedPointer<FileSignalReader> reader_2 (FileSignalReaderFactory::getInstance()->getHandler(event_file_2.fileName()));
    events = reader_2->getEvents();
    VERIFY (events.size() == event_manager->getEvents(*(exported_event_types.begin())).size(), "EVT writer 2, not all events in file")

    foreach (EventID event_id, event_manager->getEvents(*(exported_event_types.begin())))
    {
        bool event_exists = false;
        foreach (QSharedPointer<SignalEvent const> event, events)
        {
            if (event->equals(*(event_manager->getEvent(event_id))))
                event_exists = true;
        }

        VERIFY (event_exists, "loaded events")
    }

    return "";
}

}

}
