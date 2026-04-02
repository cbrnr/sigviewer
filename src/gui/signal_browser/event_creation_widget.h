// © SigViewer developers
//
// License: GPL-3.0

#ifndef EVENT_CREATION_WIDGET_H
#define EVENT_CREATION_WIDGET_H

#include <QMutex>
#include <QWidget>
#include <set>

#include "base/sigviewer_user_types.h"
#include "file_handling/event_manager.h"
#include "gui/signal_visualisation_model.h"
#include "ui_event_creation_widget.h"

namespace sigviewer {

class EventCreationWidget : public QWidget {
    Q_OBJECT
   public:
    explicit EventCreationWidget(QSharedPointer<SignalVisualisationModel> signal_visualisation_model,
        QSharedPointer<EventManager> event_manager,
        QWidget* parent = 0);

    //! When user customizes the event text, add the new event type to the combobox
    int insertNewEventType();

   signals:

    //! User added new event type
    void newEventType(std::set<EventType>);

   public slots:
    void updateShownEventTypes(std::set<EventType> const& shown_event_types);

   private slots:
    void on_type_combobox__currentIndexChanged(int combo_box_index);

    void on_pushButton_clicked();

    void on_lineEdit_returnPressed();

   private:
    void setSelfUpdating(bool self_updating);
    bool isSelfUpdating();

    QSharedPointer<SignalVisualisationModel> signal_visualisation_model_;
    QSharedPointer<EventManager> event_manager_;

    QMutex self_updating_mutex_;
    bool self_updating_;

    Ui::EventCreationWidget ui_;

    QString customized_text_;
    int customized_event_id_;
};

}  // namespace sigviewer

#endif  // EVENT_CREATION_WIDGET_H
