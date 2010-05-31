#ifndef EVENT_TABLE_EDITING_DIALOG_H
#define EVENT_TABLE_EDITING_DIALOG_H

#include "ui_event_table_dialog.h"
#include "../../file_handling/event_manager.h"
#include "../../file_handling/channel_manager.h"
#include "../../command_executer.h"

#include <QDialog>

namespace BioSig_
{

class EventTableEditingDialog : public QDialog
{
    Q_OBJECT
public:
    EventTableEditingDialog (QSharedPointer<EventManager> event_manager,
                             QSharedPointer<ChannelManager const> channel_manager,
                             QSharedPointer<CommandExecuter> command_executer,
                             std::set<EventType> const& shown_event_types);

private slots:
    void addToTable (QSharedPointer<SignalEvent const> event);
    void removeFromTable (EventID event);
    void updateTable (EventID event);

    void on_delete_button__clicked ();
    void on_all_events_button__clicked (bool checked);
    void on_shown_events_button__clicked (bool checked);

private:
    void buildTable ();

    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<ChannelManager const> channel_manager_;
    QSharedPointer<CommandExecuter> command_executer_;

    static int const ID_INDEX_ = 0;
    static int const POSITION_INDEX_ = 1;
    static int const DURATION_INDEX_ = 2;
    static int const CHANNEL_INDEX_ = 3;
    static int const TYPE_INDEX_ = 4;

    int precision_;
    std::set<EventType> shown_event_types_;

    Ui::EventTableDialog ui_;
};

}
#endif // EVENT_TABLE_EDITING_DIALOG_H
