// © SigViewer developers
//
// License: GPL-3.0

#include "file_handling/event_manager.h"
#include "file_handling/file_signal_reader_factory.h"
#include "base/signal_event.h"
#include "mock_file_signal_reader.h"
#include "editing_commands/change_channel_undo_command.h"
#include "editing_commands/change_type_undo_command.h"
#include "editing_commands/delete_event_undo_command.h"
#include "editing_commands/new_event_undo_command.h"
#include "editing_commands/resize_event_undo_command.h"
#include "base/sigviewer_user_types.h"

#include <QApplication>
#include <QUndoStack>
#include <QtTest>

using namespace sigviewer;

static QSharedPointer<EventManager> makeEventManager()
{
    FileSignalReader* reader =
        FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy");
    QSharedPointer<EventManager> mgr(new EventManager(*reader));
    delete reader;
    return mgr;
}

class TestEditingCommands : public QObject
{
    Q_OBJECT

private slots:
    void undoRedoCommands()
    {
        auto mgr = makeEventManager();
        auto event = mgr->getEvent(1);
        QVERIFY(!event.isNull());
        EventID id = event->getId();

        unsigned const initialCount = mgr->getNumberOfEvents();
        ChannelID const oldChannel   = event->getChannel();
        EventType const oldType      = event->getType();
        uint32    const oldPosition  = event->getPosition();
        uint32    const oldDuration  = event->getDuration();
        ChannelID const newChannel   = 1;
        EventType const newType      = 4;
        uint32    const newPosition  = 0;
        uint32    const newDuration  = 250;

        QUndoStack stack;

        // --- change channel ---
        stack.push(new ChangeChannelUndoCommand(mgr, id, newChannel));
        QCOMPARE(mgr->getEvent(id)->getChannel(), newChannel);
        stack.undo();
        QCOMPARE(mgr->getEvent(id)->getChannel(), oldChannel);

        // --- change type ---
        stack.push(new ChangeTypeUndoCommand(mgr, id, newType));
        QCOMPARE(mgr->getEvent(id)->getType(), newType);
        stack.undo();
        QCOMPARE(mgr->getEvent(id)->getType(), oldType);

        // --- delete event ---
        stack.push(new DeleteEventUndoCommand(mgr, id));
        QVERIFY(mgr->getEvent(id).isNull());
        QCOMPARE(mgr->getNumberOfEvents(), initialCount - 1);
        stack.undo();
        QVERIFY(!mgr->getEvent(id).isNull());
        QCOMPARE(mgr->getNumberOfEvents(), initialCount);

        // --- new event ---
        stack.push(new NewEventUndoCommand(
            mgr, QSharedPointer<SignalEvent>(new SignalEvent(10, 10, 100, 1, 100)), 1));
        QCOMPARE(mgr->getNumberOfEvents(), initialCount + 1);
        stack.undo();
        QCOMPARE(mgr->getNumberOfEvents(), initialCount);

        // --- resize event ---
        stack.push(new ResizeEventUndoCommand(mgr, id, newPosition, newDuration));
        QCOMPARE(mgr->getEvent(id)->getPosition(), newPosition);
        QCOMPARE(mgr->getEvent(id)->getDuration(), newDuration);
        stack.undo();
        QCOMPARE(mgr->getEvent(id)->getPosition(), oldPosition);
        QCOMPARE(mgr->getEvent(id)->getDuration(), oldDuration);
    }
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    sigviewer::registerMockFileSignalReader();
    TestEditingCommands test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_editing_commands.moc"
