// event_table_dialog.cpp

#include "event_table_dialog.h"

#ifndef QT4_PORTED
    #include "signal_browser_model.h"
#else
    #include "signal_browser/signal_browser_model_4.h"
#endif

#include "gui_signal_buffer.h"
#include "event_canvas_item.h"
#include "base/basic_header.h"
#include "base/event_table_file_reader.h"
#include "main_window_model.h"

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
                                              const QModelIndex & parent)
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
                                              const QModelIndex & parent)
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
#ifndef QT4_PORTED
EventTableDialog::EventTableDialog(SignalBrowserModel& browser_model,
                                   QPointer<BasicHeader> basic_header, QWidget* parent)
#else
EventTableDialog::EventTableDialog(PortingToQT4_::SignalBrowserModel& browser_model,
                                   QPointer<BasicHeader> basic_header, QWidget* parent)

#endif
 : QDialog(parent),
   signal_browser_model_(browser_model),
   basic_header_(basic_header)
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

    EventTableFileReader& event_table_reader
        = signal_browser_model_.getMainWindowModel().getEventTableFileReader();
    SignalBrowserModel::SignalEventVector event_vector;
    signal_browser_model_.getEvents(event_vector);
    int32 number_channels = (int32)basic_header_->getNumberChannels();
    float64 sample_rate = basic_header_->getEventSamplerate();
    event_table_model_->insertRows(0, event_vector.size());
    int32 row_height = event_table_view_->verticalHeader()->sizeHint().height();

    SignalBrowserModel::SignalEventVector::iterator it;
    int32 event_nr = 0;

    for (it = event_vector.begin(); it != event_vector.end(); it++, event_nr++)
    {
        event_table_view_->verticalHeader()->resizeSection(event_nr,
                                                           row_height);

        event_table_model_->setData(event_table_model_->index(event_nr, 0),
                                    QVariant(it->getId()));

        event_table_model_->setData(event_table_model_->index(event_nr, 1),
                                    QString("%1").arg(it->getPosition() /
                                                      sample_rate, 0, 'f', 1));

        event_table_model_->setData(event_table_model_->index(event_nr, 2),
                                    QString("%1").arg(it->getDuration() /
                                                      sample_rate, 0, 'f', 1));

        QString tmp;

        if (it->getChannel() == SignalEvent::UNDEFINED_CHANNEL)
        {
            tmp += tr("All Channels");
        }
        else
        {
            tmp = QString("(%1)").arg(it->getChannel() + 1) + " ";
            if (it->getChannel() < number_channels)
            {
                tmp += basic_header_->getChannel(it->getChannel()).getLabel();
            }
            else
            {
                tmp += "???";
            }
        }

        event_table_model_->setData(event_table_model_->index(event_nr, 3),
                                    QVariant(tmp));

        tmp = event_table_reader.getEventName(it->getType()) +
              QString(" ") + QString("(0x%1)").arg(it->getType(), 4, 16)
                                              .replace(' ', '0');

        event_table_model_->setData(event_table_model_->index(event_nr, 4),
                                    QVariant(tmp));

        // hidden columns for sorting
        event_table_model_->setData(event_table_model_->index(event_nr, 5),
                                    QString("%1").arg(it->getId(), 8, 16)
                                                    .replace(' ', '0'));

        event_table_model_->setData(event_table_model_->index(event_nr, 6),
                                    QString("%1").arg(it->getPosition(), 8, 16)
                                                 .replace(' ', '0'));

        event_table_model_->setData(event_table_model_->index(event_nr, 7),
                                    QString("%1").arg(it->getDuration(), 8, 16)
                                                 .replace(' ', '0'));

        event_table_model_->setData(event_table_model_->index(event_nr, 8),
                                    QString("%1").arg(it->getChannel()+1, 8, 16)
                                                 .replace(' ', '0'));

        event_table_model_->setData(event_table_model_->index(event_nr, 9),
                                    QString("%1").arg(it->getType(),4,16)
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

        do
        {
            int32 row = *(--it);
            int32 event_id = event_table_model_
                            ->data(event_table_model_->index(row, 0)).toInt();

            signal_browser_model_.removeEvent(event_id, false);
            event_table_model_->removeRows(*it, 1);
        }

        while (it != selected_row_list.begin());

        signal_browser_model_.updateLayout();
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

