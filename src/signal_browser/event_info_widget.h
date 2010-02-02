#ifndef EVENT_INFO_DOCKWIDGET_H
#define EVENT_INFO_DOCKWIDGET_H

#include "../base/signal_event.h"

#include <QWidget>
#include <QSharedPointer>
#include <QLabel>
#include <QObject>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>

#include <map>

namespace BioSig_
{

class SignalBrowserModel;

class EventInfoWidget : public QWidget
{
    Q_OBJECT

public:
    EventInfoWidget(QWidget* parent, QSharedPointer<SignalBrowserModel> signal_browser_model,
                        std::map<uint16, QString> const& shown_event_types);
    virtual ~EventInfoWidget ();

public slots:
    void updateSelectedEventInfo (QSharedPointer<SignalEvent> selected_signal_event);
    void updateCreationType (uint16 new_creation_type);

signals:
    void eventCreationTypeChanged (uint16 new_creation_type);

private slots:
    void selfChangedCreationType (int combo_box_index);
    void selfChangedType (int combo_box_index);
    void insertEvent ();

private:
    QSharedPointer<SignalBrowserModel> signal_browser_model_;
    uint16 event_creation_type_;
    std::map<uint16, QString> shown_event_types_;

    QSharedPointer<SignalEvent> selected_signal_event_;

    QLabel* id_label_;
    QLabel* duration_label_;
    QLabel* start_label_;
    QComboBox* event_creation_type_combobox_;
    QComboBox* event_type_combobox_;
    QPushButton* insert_event_button_;
    QVBoxLayout* layout_;
};

}

#endif // EVENT_INFO_DOCKWIDGET_H
