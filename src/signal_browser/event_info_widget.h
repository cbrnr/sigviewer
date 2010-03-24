#ifndef EVENT_INFO_DOCKWIDGET_H
#define EVENT_INFO_DOCKWIDGET_H

#include "../base/user_types.h"
#include "../base/signal_event.h"

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

class SignalBrowserModel;
class SignalEvent;

class EventInfoWidget : public QWidget
{
    Q_OBJECT

public:
    EventInfoWidget(QWidget* parent,
                    QSharedPointer<SignalBrowserModel> signal_browser_model);
    virtual ~EventInfoWidget ();

public slots:
    void updateSelectedEventInfo (QSharedPointer<SignalEvent> selected_signal_event);
    void updateCreationType (uint16 new_creation_type);
    void updateShownEventTypes (std::set<uint16> shown_event_types);
    void addHoveredEvent (QSharedPointer<SignalEvent const> event);
    void removeHoveredEvent (QSharedPointer<SignalEvent const> event);

signals:
    void eventCreationTypeChanged (uint16 new_creation_type);

private slots:
    void selfChangedCreationType (int combo_box_index);
    void selfChangedType (int combo_box_index);
    void selfChangedDuration (double new_duration);
    void insertEvent ();

private:
    void updateHoveredInfoLabel ();
    void setSelfUpdating (bool self_updating);
    bool isSelfUpdating ();


    QSharedPointer<SignalBrowserModel> signal_browser_model_;
//    uint16 event_creation_type_;
    std::map<uint16, QString> shown_event_types_;

    QSharedPointer<SignalEvent> selected_signal_event_;

    QList<QWidget*> disabled_widgets_if_nothing_selected_;
    QMap<int32, QSharedPointer<SignalEvent const> > hovered_events_;

//    QLabel* id_label_;
    QDoubleSpinBox* duration_spinbox_;
    QLabel* start_label_;
//    QComboBox* event_creation_type_combobox_;
    QComboBox* event_type_combobox_;
//    QPushButton* insert_event_button_;
    QHBoxLayout* layout_;
    QLabel* hovered_info_label_;

    QMutex self_updating_mutex_;
    bool self_updating_;
};

}

#endif // EVENT_INFO_DOCKWIDGET_H
