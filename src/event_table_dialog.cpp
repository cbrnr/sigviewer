// event_table_dialog.cpp

#include "event_table_dialog.h"

#include "command_executer.h"
#include "file_handling/basic_header.h"
#include "file_handling/event_manager.h"
#include "signal_browser/delete_multiple_events_undo_command.h"

#include <QHeaderView>
#include <QTableView>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>

namespace BioSig_
{

// table model constructor
EventTableDialog::TableModel::TableModel(QObject* parent)
: QAbstractTableModel(parent)
{
    column_sort_oder_[0] = Qt::AscendingOrder;
    column_sort_oder_[1] = Qt::AscendingOrder;
    column_sort_oder_[2] = Qt::AscendingOrder;
    column_sort_oder_[3] = Qt::AscendingOrder;
    column_sort_oder_[4] = Qt::AscendingOrder;
}

// table model row count
int EventTableDialog::TableModel::rowCount(const QModelIndex&) const
{
    return data_.size();
}

// table model column count
int EventTableDialog::TableModel::columnCount(const QModelIndex&) const
{
    return 10;
}

// table model data
QVariant EventTableDialog::TableModel
::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole ||
        index.row() < 0 || index.row() >= data_.size() ||
        index.column() < 0 || index.column() >= 10)
    {
        return QVariant();
    }

    return data_.at(index.row()).data[index.column()];
}

// table model flags
Qt::ItemFlags EventTableDialog::TableModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

// table model insert rows
bool EventTableDialog::TableModel::insertRows(int row, int count,
                                              const QModelIndex &)
{
    if (row < 0 || count < 0 || row > data_.size())
    {
        return false;
    }

    beginInsertRows(QModelIndex(), row, row + count - 1);

    while (count-- > 0)
    {
        data_.insert(row, RowData());
    }

    endInsertRows();

    return true;
}

// table model remove rows
bool EventTableDialog::TableModel::removeRows(int row, int count,
                                              const QModelIndex &)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);

    int ok = false;

    if (row >= 0 && count >= 0 && row + count <= data_.size())
    {
        while (count-- > 0)
        {
            data_.removeAt(row);
        }
        ok = true;
    }

    endRemoveRows();

    return ok;
}

// table model set data
bool EventTableDialog::TableModel::setData(const QModelIndex &index,
                                           const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole ||
        index.row() < 0 || index.row() >= data_.size() ||
        index.column() < 0 || index.column() >= 10)
    {
        return false;
    }

    data_[index.row()].data[index.column()] = value.toString();
    emit dataChanged(index, index);

    return true;
}

// table model header data
QVariant EventTableDialog::TableModel::headerData(int section,
                                                  Qt::Orientation orientation,
                                                  int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
            case 0:
                return tr("ID");
            case 1:
                return tr("Position");
            case 2:
                return tr("Duration");
            case 3:
                return tr("Channel");
            case 4:
                return tr("Type");
            default:
                break;
        }
    }

    return QString("%1").arg(section + 1);
}

// table model get sort order
Qt::SortOrder EventTableDialog::TableModel::getSortOrder(int column)
{
    if (column < 0 || column > 4)
    {
        return Qt::AscendingOrder;
    }

    return column_sort_oder_[column];
}

// table model sort
void EventTableDialog::TableModel::sort(int column, Qt::SortOrder order)
{
    column_sort_oder_[column] = order;

    if (order == Qt::AscendingOrder)
    {
        switch(column)
        {
            case 0:
                qSort(data_.begin(), data_.end(), sortCompare<0, true>);
                break;
            case 1:
                qSort(data_.begin(), data_.end(), sortCompare<1, true>);
                break;
            case 2:
                qSort(data_.begin(), data_.end(), sortCompare<2, true>);
                break;
            case 3:
                qSort(data_.begin(), data_.end(), sortCompare<3, true>);
                break;
            case 4:
                qSort(data_.begin(), data_.end(), sortCompare<4, true>);
                break;
            default:
                break;
        }
    }
    else
    {
        switch(column)
        {
            case 0:
                qSort(data_.begin(), data_.end(), sortCompare<0, false>);
                break;
            case 1:
                qSort(data_.begin(), data_.end(), sortCompare<1, false>);
                break;
            case 2:
                qSort(data_.begin(), data_.end(), sortCompare<2, false>);
                break;
            case 3:
                qSort(data_.begin(), data_.end(), sortCompare<3, false>);
                break;
            case 4:
                qSort(data_.begin(), data_.end(), sortCompare<4, false>);
                break;
            default:
                break;
        }
    }

    emit dataChanged(index(0, 0), index(data_.size() - 1, 9));
}

// constructor
EventTableDialog::EventTableDialog (EventManager& event_manager,
                                    CommandExecuter& command_executer,
                                    QPointer<BasicHeader> basic_header, QWidget* parent)
 : QDialog(parent),
   event_manager_ (event_manager),
   command_executer_ (command_executer),
   basic_header_ (basic_header)
{
    setWindowTitle(tr("Events"));
    QHBoxLayout* top_layout = new QHBoxLayout(this);
    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    event_table_view_ = new QTableView(this);
    top_layout->addWidget(event_table_view_);
    event_table_model_ = new TableModel(this);
    event_table_view_->setModel(event_table_model_);
    QVBoxLayout* button_layout = new QVBoxLayout;
    button_layout->setMargin(10);
    top_layout->addLayout(button_layout);
    remove_button_ = new QPushButton("Remove", this);
    button_layout->addWidget(remove_button_);
    button_layout->addStretch(1);
    close_button_ = new QPushButton(tr("Close"), this);
    button_layout->addWidget(close_button_);
    buildEventTable();
    top_layout->activate();

    connect(close_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(remove_button_, SIGNAL(clicked()),
            this, SLOT(removeSelectedRows()));
    connect(event_table_view_->horizontalHeader(),
            SIGNAL(sectionDoubleClicked(int)),
            this, SLOT(columnHeaderClicked(int)));
}

// load settings
void EventTableDialog::loadSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("EventTableDialog");
    resize(settings.value("size", QSize(800, 500)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());

    event_table_view_->horizontalHeader()->resizeSection(0, settings.value("col0_width", 75).toInt());
    event_table_view_->horizontalHeader()->resizeSection(1, settings.value("col1_width", 75).toInt());
    event_table_view_->horizontalHeader()->resizeSection(2, settings.value("col2_width", 75).toInt());
    event_table_view_->horizontalHeader()->resizeSection(3, settings.value("col3_width", 150).toInt());
    event_table_view_->horizontalHeader()->resizeSection(4, settings.value("col4_width", 300).toInt());

    settings.endGroup();
}

// save settings
void EventTableDialog::saveSettings()
{
    QSettings settings("SigViewer");

    settings.beginGroup("EventTableDialog");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("col0_width", event_table_view_->horizontalHeader()->sectionSize(0));
    settings.setValue("col1_width", event_table_view_->horizontalHeader()->sectionSize(1));
    settings.setValue("col2_width", event_table_view_->horizontalHeader()->sectionSize(2));
    settings.setValue("col3_width", event_table_view_->horizontalHeader()->sectionSize(3));
    settings.setValue("col4_width", event_table_view_->horizontalHeader()->sectionSize(4));
    settings.endGroup();
}

// build event table
void EventTableDialog::buildEventTable()
{
    // Qt-bug ??? : should be QAbstractItemView::MultiSelection
    event_table_view_->setSelectionMode(QAbstractItemView::MultiSelection);
    event_table_view_->setSelectionBehavior(QAbstractItemView::SelectRows);

    // hidden columns for sorting
    event_table_view_->setColumnHidden(5, true);
    event_table_view_->setColumnHidden(6, true);
    event_table_view_->setColumnHidden(7, true);
    event_table_view_->setColumnHidden(8, true);
    event_table_view_->setColumnHidden(9, true);
    event_table_view_->horizontalHeader()->setClickable(false);
    event_table_view_->horizontalHeader()->setSortIndicatorShown(true);

    QList<EventID> event_ids = event_manager_.getAllEvents ();

    int32 number_channels = (int32)basic_header_->getNumberChannels();
    float64 sample_rate = basic_header_->getEventSamplerate();
    event_table_model_->insertRows(0, event_ids.size());
    int32 row_height = event_table_view_->verticalHeader()->sizeHint().height();

    for (QList<EventID>::Iterator id_iter = event_ids.begin ();
         id_iter != event_ids.end ();
         ++id_iter)
    {
        QSharedPointer<SignalEvent const> event =
                event_manager_.getEvent (*id_iter);

        event_table_view_->verticalHeader()->resizeSection(*id_iter,
                                                           row_height);

        event_table_model_->setData(event_table_model_->index(*id_iter, 0),
                                    event->getId());

        event_table_model_->setData(event_table_model_->index(*id_iter, 1),
                                    QString("%1").arg(event->getPosition() /
                                                      sample_rate, 0, 'f', 2));

        event_table_model_->setData(event_table_model_->index(*id_iter, 2),
                                    QString("%1").arg(event->getDuration() /
                                                      sample_rate, 0, 'f', 2));

        QString tmp;

        if (event->getChannel() == SignalEvent::UNDEFINED_CHANNEL)
        {
            tmp += tr("All Channels");
        }
        else
        {
            tmp = QString("(%1)").arg(event->getChannel() + 1) + " ";
            if (event->getChannel() < number_channels)
            {
                tmp += basic_header_->getChannel(event->getChannel()).getLabel();
            }
            else
            {
                tmp += "???";
            }
        }

        event_table_model_->setData(event_table_model_->index(*id_iter, 3),
                                    QVariant(tmp));

        tmp = event_manager_.getNameOfEventType(event->getType()) +
              QString(" ") + QString("(0x%1)").arg(event->getType(), 4, 16)
                                              .replace(' ', '0');

        event_table_model_->setData(event_table_model_->index(*id_iter, 4),
                                    QVariant(tmp));

        // hidden columns for sorting
        event_table_model_->setData(event_table_model_->index(*id_iter, 5),
                                    QString("%1").arg(event->getId(), 8, 16)
                                                    .replace(' ', '0'));

        event_table_model_->setData(event_table_model_->index(*id_iter, 6),
                                    QString("%1").arg(event->getPosition(), 8, 16)
                                                 .replace(' ', '0'));

        event_table_model_->setData(event_table_model_->index(*id_iter, 7),
                                    QString("%1").arg(event->getDuration(), 8, 16)
                                                 .replace(' ', '0'));

        event_table_model_->setData(event_table_model_->index(*id_iter, 8),
                                    QString("%1").arg(event->getChannel()+1, 8, 16)
                                                 .replace(' ', '0'));

        event_table_model_->setData(event_table_model_->index(*id_iter, 9),
                                    QString("%1").arg(event->getType(),4,16)
                                                    .replace(' ', '0'));
    }
}

// remove selected rows
void EventTableDialog::removeSelectedRows()
{
    QList<int32> selected_row_list;

    for (int32 row = event_table_model_->rowCount() - 1; row >= 0; row--)
    {
        if (event_table_view_->selectionModel()
                             ->isSelected(event_table_model_->index(row, 0)))
        {
            selected_row_list << row;
        }
    }

    if (selected_row_list.size() > 0)
    {
        qSort(selected_row_list);
        QList<int32>::iterator it = selected_row_list.end();
        QList<EventID> event_ids;

        do
        {
            int32 row = *(--it);
            event_ids.append (event_table_model_
                              ->data(event_table_model_->index(row, 0)).toInt());
            event_table_model_->removeRows (*it, 1);
        }
        while (it != selected_row_list.begin());

        DeleteMultipleEventsUndoCommand* delete_command =
                new DeleteMultipleEventsUndoCommand (event_manager_, event_ids);
        command_executer_.executeCommand (delete_command);
    }

    // update row heights
    int32 row_height = event_table_view_->verticalHeader()->sizeHint().height();

    for (int32 row = 0; row < event_table_model_->rowCount(); row++)
    {
        event_table_view_->verticalHeader()->resizeSection(row, row_height);
    }
}

// column header clicked
void EventTableDialog::columnHeaderClicked(int column)
{
    event_table_view_->clearSelection();
    Qt::SortOrder sort_order = Qt::AscendingOrder;

    if (event_table_model_->getSortOrder(column) == Qt::AscendingOrder)
    {
           sort_order = Qt::DescendingOrder;
    }

    event_table_model_->sort(column, sort_order);
    event_table_view_->horizontalHeader()->setSortIndicator(column, sort_order);
}

} //namespace BioSig_

