// event_table_dialog.h

#ifndef EVENT_TABLE_DIALOG_H
#define EVENT_TABLE_DIALOG_H

#include "../base/user_types.h"
#include "../command_executer.h"
#include "../file_handling/event_manager.h"
#include "../file_handling/channel_manager.h"

#include <qdialog.h>
#include <QAbstractTableModel>
#include <QPointer>
#include <QSharedPointer>

class QPushButton;
class QTableView;

namespace BioSig_
{


// event table dialog
class EventTableDialog : public QDialog
{
    Q_OBJECT
public:
    EventTableDialog (QSharedPointer<EventManager> event_manager,
                      QSharedPointer<CommandExecuter> command_executer,
                      QSharedPointer<ChannelManager> channel_manager);

    void loadSettings();
    void saveSettings();

private slots:
    void removeSelectedRows();
    void columnHeaderClicked(int column);

private:
    // table model
    class TableModel : public QAbstractTableModel
    {
    public:
        TableModel(QObject* parent = 0);

        int rowCount(const QModelIndex& parent = QModelIndex()) const ;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
        QVariant data(const QModelIndex& index,
                      int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags( const QModelIndex&  index) const;
        bool insertRows(int row, int count,
                        const QModelIndex & parent = QModelIndex());
        bool removeRows(int row, int count,
                        const QModelIndex & parent = QModelIndex());
        bool setData(const QModelIndex &index, const QVariant &value,
                     int role = Qt::EditRole);
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
        void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
        Qt::SortOrder getSortOrder(int column);

    private:
        struct RowData
        {
            QString data[10];
        };

        template<int column, bool ascending>
        static bool sortCompare(const RowData& rd1, const RowData& rd2)
        {
            if (ascending)
            {
                return rd1.data[column + 5] < rd2.data[column + 5];
            }
            else
            {
                return rd1.data[column + 5] > rd2.data[column + 5];
            }
        }
    
        QList<RowData> data_;
        Qt::SortOrder column_sort_oder_[5];   
    };

    // not allowed
    EventTableDialog();
    EventTableDialog(const EventTableDialog&);
    const EventTableDialog& operator=(const EventTableDialog&);

    void buildEventTable();

    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<CommandExecuter> command_executer_;
    QSharedPointer<ChannelManager> channel_manager_;
    QTableView *event_table_view_;
    TableModel *event_table_model_;
    QPushButton* close_button_;
    QPushButton* remove_button_;
};

} // namespace BioSig_

#endif