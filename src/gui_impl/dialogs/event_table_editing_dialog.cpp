#include "event_table_editing_dialog.h"
#include "gui/gui_action_factory.h"
#include "editing_commands/delete_event_undo_command.h"
#include "editing_commands/macro_undo_command.h"
#include "base/math_utils.h"

#include <QDebug>

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
EventTableEditingDialog::EventTableEditingDialog (QSharedPointer<EventManager> event_manager,
                                                  QSharedPointer<ChannelManager const> channel_manager,
                                                  QSharedPointer<CommandExecuter> command_executer,
                                                  std::set<EventType> const& shown_event_types) :
event_manager_ (event_manager),
channel_manager_ (channel_manager),
command_executer_ (command_executer),
precision_ (MathUtils_::sampleRateToDecimalPrecision(event_manager->getSampleRate())),
shown_event_types_ (shown_event_types)
{
    ui_.setupUi (this);
    buildTable ();
    ui_.event_table_->hideColumn (ID_INDEX_);
    connect (event_manager_.data(), SIGNAL(eventRemoved(EventID)), SLOT(removeFromTable (EventID)));
    connect (event_manager_.data(), SIGNAL(eventCreated(QSharedPointer<SignalEvent const>)), SLOT(addToTable(QSharedPointer<SignalEvent const>)));
    connect (event_manager_.data(), SIGNAL(eventChanged(EventID)), SLOT(updateTable(EventID)));
    ui_.undo_button_->setDefaultAction(GuiActionFactory::getInstance()->getQAction("Undo"));
    ui_.redo_button_->setDefaultAction(GuiActionFactory::getInstance()->getQAction("Redo"));
}

//-------------------------------------------------------------------------
void EventTableEditingDialog::addToTable (QSharedPointer<SignalEvent const> event)
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
void EventTableEditingDialog::removeFromTable (EventID event)
{
    qDebug () << "EventTableEditingDialog::removeFromTable " << event;
    int row_to_remove = -1;
    for (int row = 0; row < ui_.event_table_->rowCount(); row++)
        if (ui_.event_table_->item (row, ID_INDEX_)->text().toInt()
            == event)
            row_to_remove = row;
    if (row_to_remove != -1)
        ui_.event_table_->removeRow (row_to_remove);
    qDebug () << "EventTableEditingDialog::removeFromTable " << event << " in row " << row_to_remove << " finished";
}


//-------------------------------------------------------------------------
void EventTableEditingDialog::updateTable (EventID event)
{
    QSharedPointer<SignalEvent const> signal_event = event_manager_->getEvent (event);
    for (int row = 0; row < ui_.event_table_->rowCount(); row++)
        if (ui_.event_table_->item (row, ID_INDEX_)->text().toInt()
            == event)
        {
            ui_.event_table_->item (row, CHANNEL_INDEX_)->setText (channel_manager_->getChannelLabel(signal_event->getChannel()));
        }
}

//-------------------------------------------------------------------------
void EventTableEditingDialog::on_delete_button__clicked ()
{
    std::set<EventID> events_to_delete;
    QList<QSharedPointer<QUndoCommand> > delete_commands;
    foreach (QTableWidgetItem* item, ui_.event_table_->selectedItems())
        events_to_delete.insert (ui_.event_table_->item(ui_.event_table_->row(item), ID_INDEX_)->text().toInt());

    foreach (EventID event, events_to_delete)
        delete_commands.push_back (QSharedPointer<QUndoCommand>(new DeleteEventUndoCommand (event_manager_, event)));

    MacroUndoCommand* macro_command (new MacroUndoCommand (delete_commands));
    command_executer_->executeCommand (macro_command);
}

//-------------------------------------------------------------------------
void EventTableEditingDialog::on_all_events_button__clicked (bool checked)
{
    if (!checked)
        return;

    for (int row = 0; row < ui_.event_table_->rowCount(); row++)
        ui_.event_table_->showRow (row);
}

//-------------------------------------------------------------------------
void EventTableEditingDialog::on_shown_events_button__clicked (bool checked)
{
    if (!checked)
        return;

    for (int row = 0; row < ui_.event_table_->rowCount(); row++)
        if (shown_event_types_.count (event_manager_->getEvent (ui_.event_table_->item (row, ID_INDEX_)->text().toInt())->getType()) == 0)
            ui_.event_table_->hideRow (row);
}

//-------------------------------------------------------------------------
void EventTableEditingDialog::buildTable ()
{
    foreach (EventID event_id, event_manager_->getAllEvents ())
        addToTable (event_manager_->getEvent (event_id));
}


}
