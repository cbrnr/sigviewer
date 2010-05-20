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
                             QSharedPointer<CommandExecuter> command_executer);

private slots:
    void addToTable (QSharedPointer<SignalEvent const> event);
    void removeFromTable (EventID event);

    void on_delete_button__clicked ();
private:
    void buildTable ();

    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<ChannelManager const> channel_manager_;
    QSharedPointer<CommandExecuter> command_executer_;

    static int const POSITION_INDEX_ = 0;
    static int const DURATION_INDEX_ = 1;
    static int const CHANNEL_INDEX_ = 2;
    static int const TYPE_INDEX_ = 3;
    static int const ID_INDEX_ = 4;


    Ui::EventTableDialog ui_;
};

}
#endif // EVENT_TABLE_EDITING_DIALOG_H
