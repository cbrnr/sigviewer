#ifndef EVENT_INFO_DOCKWIDGET_H
#define EVENT_INFO_DOCKWIDGET_H

#include "../base/user_types.h"
#include "../base/signal_event.h"
#include "../command_executer.h"
#include "../file_handling/event_manager.h"

#include <QWidget>
#include <QSharedPointer>
#include <QLabel>
#include <QObject>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QList>
#include <QMap>
#include <QMutex>

#include <map>
#include <set>

namespace BioSig_
{

class SignalEvent;

class EventInfoWidget : public QWidget
{
    Q_OBJECT

public:
    EventInfoWidget(QWidget* parent,
                    QSharedPointer<EventManager> event_manager,
                    QSharedPointer<CommandExecuter> command_executer);
    virtual ~EventInfoWidget ();

public slots:
    void updateSelectedEventInfo (QSharedPointer<SignalEvent const>
                                  selected_signal_event);
    void updateShownEventTypes (std::set<uint16> shown_event_types);

private slots:
    void selfChangedType (int combo_box_index);
    void selfChangedDuration (double new_duration);

private:
    void setSelfUpdating (bool self_updating);
    bool isSelfUpdating ();

    QSharedPointer<EventManager> event_manager_;
    QSharedPointer<CommandExecuter> command_executer_;
    std::map<uint16, QString> shown_event_types_;

    QSharedPointer<SignalEvent const> selected_signal_event_;

    QList<QWidget*> disabled_widgets_if_nothing_selected_;
    QMap<int32, QSharedPointer<SignalEvent const> > hovered_events_;

    QDoubleSpinBox* duration_spinbox_;
    QLabel* start_label_;
    QComboBox* event_type_combobox_;
    QHBoxLayout* layout_;

    QMutex self_updating_mutex_;
    bool self_updating_;
};

}

#endif // EVENT_INFO_DOCKWIDGET_H
