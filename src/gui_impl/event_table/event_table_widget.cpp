#include "event_table_widget.h"


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


EventTableWidget::EventTableWidget (QSharedPointer<EventManager> event_manager,
                                   QSharedPointer<ChannelManager const> channel_manager,
                                   QWidget *parent) :
    QWidget(parent),
    precision_ (0),
    event_manager_ (event_manager),
    channel_manager_ (channel_manager)
{
    for (float32 sample_rate = event_manager_->getSampleRate(); sample_rate > 10; sample_rate /= 10)
        precision_++;

    ui_.setupUi(this);
    buildTable();
}

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
    QTableWidgetItem* channel_item = new QTableWidgetItem (channel_manager_->getChannelLabel(event->getChannel()));
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
void EventTableWidget::buildTable ()
{
    foreach (EventID event_id, event_manager_->getAllEvents ())
        addToTable (event_manager_->getEvent (event_id));
}

}
