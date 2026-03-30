// © SigViewer developers
//
// License: GPL-3.0

#include "gui/event_table/event_table_widget.h"
#include "file_handling/event_manager.h"
#include "file_handling/file_channel_manager.h"
#include "file_handling/file_signal_reader_factory.h"
#include "tab_context.h"

#include <QApplication>
#include <QTableWidget>
#include <QtTest>

using namespace sigviewer;

// Forward-declared as friend in EventTableWidget.
class TestEventTableWidget : public QObject
{
    Q_OBJECT

    static QSharedPointer<EventManager> makeEventManager()
    {
        FileSignalReader* reader =
            FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy");
        QSharedPointer<EventManager> mgr(new EventManager(*reader));
        delete reader;
        return mgr;
    }

    static FileChannelManager* makeChannelManager()
    {
        return new FileChannelManager(
            FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy"));
    }

private slots:
    void basicCreation()
    {
        auto mgr = makeEventManager();
        QScopedPointer<FileChannelManager> cm(makeChannelManager());
        EventTableWidget w(QSharedPointer<TabContext>(new TabContext()), mgr, *cm);
        QCOMPARE(w.ui_.event_table_->rowCount(),
                 static_cast<int>(mgr->getNumberOfEvents()));
    }

    void newEventAppearsInTable()
    {
        auto mgr = makeEventManager();
        QScopedPointer<FileChannelManager> cm(makeChannelManager());
        EventTableWidget w(QSharedPointer<TabContext>(new TabContext()), mgr, *cm);
        QTableWidget* table = w.ui_.event_table_;
        int oldCount = table->rowCount();

        auto event = mgr->createEvent(1, 10, 12, 0x03, UNDEFINED_STREAM_ID);
        QCOMPARE(table->rowCount(), oldCount + 1);

        bool found = false;
        for (int row = 0; row < table->rowCount(); ++row) {
            if (table->item(row, EventTableWidget::ID_INDEX_)->text()
                    == QString::number(event->getId())) {
                found = true;
                QCOMPARE(table->item(row, EventTableWidget::CHANNEL_INDEX_)->text(),
                         cm->getChannelLabel(1));
            }
        }
        QVERIFY(found);
    }

    void deletedEventRemovedFromTable()
    {
        auto mgr = makeEventManager();
        QScopedPointer<FileChannelManager> cm(makeChannelManager());
        EventTableWidget w(QSharedPointer<TabContext>(new TabContext()), mgr, *cm);
        QTableWidget* table = w.ui_.event_table_;
        int oldCount = table->rowCount();

        const EventID removeId = 2;
        mgr->removeEvent(removeId);
        QCOMPARE(table->rowCount(), oldCount - 1);

        for (int row = 0; row < table->rowCount(); ++row)
            QVERIFY(table->item(row, EventTableWidget::ID_INDEX_)->text()
                    != QString::number(removeId));
    }

    void changedEventReflectedInTable()
    {
        auto mgr = makeEventManager();
        QScopedPointer<FileChannelManager> cm(makeChannelManager());
        EventTableWidget w(QSharedPointer<TabContext>(new TabContext()), mgr, *cm);
        QTableWidget* table = w.ui_.event_table_;
        int oldCount = table->rowCount();

        const EventID id      = 4;
        const EventType newType    = mgr->getEvent(id)->getType() + 1;
        const ChannelID newChannel = 3;
        {
            auto ev = mgr->getAndLockEventForEditing(id);
            ev->setType(newType);
            ev->setChannel(newChannel);
            mgr->updateAndUnlockEvent(id);
        }

        QCOMPARE(table->rowCount(), oldCount);
        for (int row = 0; row < table->rowCount(); ++row) {
            if (table->item(row, EventTableWidget::ID_INDEX_)->text().toInt() == id) {
                QCOMPARE(table->item(row, EventTableWidget::TYPE_INDEX_)->text(),
                         mgr->getNameOfEventType(newType));
                QCOMPARE(table->item(row, EventTableWidget::CHANNEL_INDEX_)->text(),
                         cm->getChannelLabel(newChannel));
            }
        }
    }
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    TestEventTableWidget test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_event_table_widget.moc"
