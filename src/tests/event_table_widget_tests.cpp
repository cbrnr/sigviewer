#include "event_table_widget_tests.h"
#include "gui_impl/event_table/event_table_widget.h"

namespace SigViewer_
{

namespace Tests_
{

//-----------------------------------------------------------------------------
QString EventTableWidgetTests::run ()
{
    QSharedPointer<EventManager> event_manager = createEventManagerWithDummyData();
    RUN_SUB_TEST (basicCreation (event_manager));
    RUN_SUB_TEST (newEvents (event_manager));
    RUN_SUB_TEST (deleteEvents (event_manager));
    RUN_SUB_TEST (changedEvents (event_manager));

    return "";
}


//-----------------------------------------------------------------------------
QString EventTableWidgetTests::basicCreation (QSharedPointer<EventManager> event_manager)
{
    EventTableWidget event_table (QSharedPointer<TabContext>(new TabContext()), event_manager, getChannelManagerDummyData());
    VERIFY (static_cast<unsigned>(event_table.ui_.event_table_->rowCount()) == event_manager->getNumberOfEvents(), "number events");

    return "";
}

//-----------------------------------------------------------------------------
QString EventTableWidgetTests::newEvents (QSharedPointer<EventManager> event_manager)
{
    ChannelID const CHANNEL = 1;
    unsigned const POS = 10;
    unsigned const LENGTH = 12;
    EventType const TYPE = 0x03;

    EventTableWidget event_table (QSharedPointer<TabContext>(new TabContext()), event_manager, getChannelManagerDummyData());
    QTableWidget* table = event_table.ui_.event_table_;
    int old_row_count = table->rowCount();

    QSharedPointer<SignalEvent const> event = event_manager->createEvent (CHANNEL, POS, LENGTH, TYPE);

    VERIFY (old_row_count + 1 == table->rowCount(), "one event more");

    bool contains_new_event = false;
    for (int row = 0; row < table->rowCount(); row++)
    {
        QTableWidgetItem* table_item = table->item(row, EventTableWidget::ID_INDEX_);
        if (table_item->text() == QString::number(event->getId()))
        {
            contains_new_event = true;
            VERIFY (table->item(row, EventTableWidget::CHANNEL_INDEX_)->text() == getChannelManagerDummyData().getChannelLabel(CHANNEL), "channel fits");
        }
    }
    VERIFY (contains_new_event, "contains new event");

    return "";
}

//-----------------------------------------------------------------------------
QString EventTableWidgetTests::deleteEvents (QSharedPointer<EventManager> event_manager)
{
    EventID const EVENT_ID = 2;

    EventTableWidget event_table (QSharedPointer<TabContext>(new TabContext()), event_manager, getChannelManagerDummyData());
    QTableWidget* table = event_table.ui_.event_table_;
    int old_row_count = table->rowCount();

    event_manager->removeEvent (EVENT_ID);
    VERIFY (old_row_count - 1 == table->rowCount(), "one event less");

    bool contains_deleted_event = false;
    for (int row = 0; row < table->rowCount(); row++)
        if (table->item(row, EventTableWidget::ID_INDEX_)->text() == QString::number(EVENT_ID))
            contains_deleted_event = true;

    VERIFY (!contains_deleted_event, "not contains deleted event");

    return "";
}

//-----------------------------------------------------------------------------
QString EventTableWidgetTests::changedEvents (QSharedPointer<EventManager> event_manager)
{
    EventID const EVENT_ID = 4;
    EventType const OLD_TYPE = event_manager->getEvent (EVENT_ID)->getType();
    EventType const NEW_TYPE = OLD_TYPE + 1;
    ChannelID const NEW_CHANNEL = 3;

    EventTableWidget event_table (QSharedPointer<TabContext>(new TabContext()), event_manager, getChannelManagerDummyData());
    QTableWidget* table = event_table.ui_.event_table_;
    int old_row_count = table->rowCount();

    {
        QSharedPointer<SignalEvent> event_for_editing = event_manager->getAndLockEventForEditing (EVENT_ID);
        event_for_editing->setType (NEW_TYPE);
        event_for_editing->setChannel (NEW_CHANNEL);
        event_manager->updateAndUnlockEvent (EVENT_ID);
    }

    VERIFY (old_row_count == table->rowCount(), "same amount fo events");

    for (int row = 0; row < table->rowCount(); row++)
    {
        if (table->item(row, EventTableWidget::ID_INDEX_)->text().toInt() == EVENT_ID)
        {
            VERIFY (table->item(row, EventTableWidget::TYPE_INDEX_)->text() == event_manager->getNameOfEventType(NEW_TYPE), "types match");
            VERIFY (table->item(row, EventTableWidget::CHANNEL_INDEX_)->text() == getChannelManagerDummyData ().getChannelLabel (NEW_CHANNEL), "channels match");
        }
    }

    return "";
}


}

}
