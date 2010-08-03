#ifndef EVENT_EDITING_WIDGET_H
#define EVENT_EDITING_WIDGET_H

#include "base/sigviewer_user_types.h"
#include "file_handling/event_manager.h"
#include "command_executer.h"
#include "ui_event_editing_widget.h"

#include <QWidget>
#include <QMutex>

#include <set>

namespace SigViewer_
{

class EventEditingWidget : public QWidget
{
Q_OBJECT
public:
    explicit EventEditingWidget (QSharedPointer<EventManager> event_manager,
                                 QSharedPointer<CommandExecuter> command_executer,
                                 QWidget *parent = 0);

signals:

public slots:
    void updateShownEventTypes (std::set<EventType> const& shown_event_types);
    void updateSelectedEventInfo (QSharedPointer<SignalEvent const>
                                  selected_signal_event);


private slots:
    void on_type_combobox__currentIndexChanged (int combo_box_index);
    void on_begin_spinbox__editingFinished ();
    void on_duration_spinbox__editingFinished ();

private:
    void setSelfUpdating (bool self_updating);
    bool isSelfUpdating ();

    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<CommandExecuter> command_executer_;

    QSharedPointer<SignalEvent const> selected_signal_event_;

    QMutex self_updating_mutex_;
    bool self_updating_;

    Ui::EventEditingWidget ui_;
};

}

#endif // EVENT_EDITING_WIDGET_H
