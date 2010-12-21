#include "event_table_widget.h"
#include "base/math_utils.h"


namespace SigViewer_
{

class EventTableItem : public QTableWidgetItem
{
public:
    EventTableItem (float32 number, int precision = 0) : QTableWidgetItem (QString::number(number, 'f', precision)) {}
    virtual bool operator< (QTableWidgetItem const& other) const
    {
        return other.text().toFloat() < text().toFloat();
    }
};


//-------------------------------------------------------------------------
EventTableWidget::EventTableWidget (QSharedPointer<EventManager> event_manager,
                                   ChannelManager const& channel_manager,
                                   QWidget *parent) :
    QWidget(parent),
    precision_ (MathUtils_::sampleRateToDecimalPrecision(event_manager->getSampleRate())),
    event_manager_ (event_manager),
    channel_manager_ (channel_manager)
{
    ui_.setupUi(this);
    connect (event_manager_.data(), SIGNAL(eventCreated(QSharedPointer<const SignalEvent>)), SLOT(addToTable(QSharedPointer<const SignalEvent>)));
    connect (event_manager_.data(), SIGNAL(eventRemoved(EventID)), SLOT(removeFromTable(EventID)));
    connect (event_manager_.data(), SIGNAL(eventChanged(EventID)), SLOT(updateEventEntry(EventID)));
    buildTable();
    ui_.event_table_->hideColumn (ID_INDEX_);
}

//-------------------------------------------------------------------------
EventTableWidget::~EventTableWidget()
{

}

//-------------------------------------------------------------------------
void EventTableWidget::addToTable (QSharedPointer<SignalEvent const> event)
{
    int row = ui_.event_table_->rowCount ();
    ui_.event_table_->insertRow (row);

    QTableWidgetItem* position_item = new EventTableItem (event->getPositionInSec(), precision_);
    QTableWidgetItem* duration_item = new EventTableItem (event->getDurationInSec(), precision_);
    QTableWidgetItem* channel_item = new QTableWidgetItem (channel_manager_.getChannelLabel(event->getChannel()));
    QTableWidgetItem* type_item = new QTableWidgetItem (event_manager_->getNameOfEvent(event->getId()));
    QTableWidgetItem* id_item = new EventTableItem (event->getId());

    ui_.event_table_->setItem (row, POSITION_INDEX_,
                               position_item);
    row = ui_.event_table_->row (position_item);
    ui_.event_table_->setItem (row, DURATION_INDEX_,
                               duration_item);
    row = ui_.event_table_->row (duration_item);
    ui_.event_table_->setItem (row, CHANNEL_INDEX_,
                               channel_item);
    row = ui_.event_table_->row (channel_item);
    ui_.event_table_->setItem (row, TYPE_INDEX_,
                               type_item);
    row = ui_.event_table_->row (type_item);
    ui_.event_table_->setItem (row, ID_INDEX_,
                               id_item);
}

//-------------------------------------------------------------------------
void EventTableWidget::removeFromTable (EventID event)
{
    QList<int> rows_to_remove;
    for (int row = 0; row < ui_.event_table_->rowCount(); row++)
        if (ui_.event_table_->item(row, ID_INDEX_)->text().toInt() == event)
            rows_to_remove.prepend (row);

    foreach (int row, rows_to_remove)
        ui_.event_table_->removeRow (row);
}

//-------------------------------------------------------------------------
void EventTableWidget::updateEventEntry (EventID event_id)
{
    QSharedPointer<SignalEvent const> event = event_manager_->getEvent (event_id);
    bool updated = false;
    for (int row = 0;
         (row < ui_.event_table_->rowCount()) && (!updated);
         row++)
    {
        QTableWidgetItem* id_item = ui_.event_table_->item (row, ID_INDEX_);
        if (id_item->text().toInt() == event_id)
        {
            ui_.event_table_->item (row, CHANNEL_INDEX_)->setText (channel_manager_.getChannelLabel (event->getChannel()));
            ui_.event_table_->item (row, POSITION_INDEX_)->setText (QString::number (event->getPositionInSec(), 'f', precision_));
            ui_.event_table_->item (row, DURATION_INDEX_)->setText (QString::number (event->getDurationInSec(), 'f', precision_));
            ui_.event_table_->item (row, TYPE_INDEX_)->setText (event_manager_->getNameOfEvent (event_id));
            updated = true;
        }
    }
}

//-------------------------------------------------------------------------
void EventTableWidget::buildTable ()
{
    foreach (EventID event_id, event_manager_->getAllEvents ())
        addToTable (event_manager_->getEvent (event_id));
}


}
