#ifndef EVENT_CREATION_WIDGET_H
#define EVENT_CREATION_WIDGET_H

#include "base/sigviewer_user_types.h"
#include "ui_event_creation_widget.h"
#include "gui/signal_visualisation_model.h"
#include "file_handling/event_manager.h"

#include <QWidget>
#include <QMutex>

#include <set>

namespace SigViewer_
{

class EventCreationWidget : public QWidget
{
Q_OBJECT
public:
    explicit EventCreationWidget (QSharedPointer<SignalVisualisationModel> signal_visualisation_model,
                                  QSharedPointer<EventManager> event_manager,
                                  QWidget* parent = 0);

signals:

public slots:
    void updateShownEventTypes (std::set<EventType> const& shown_event_types);

private slots:
    void on_type_combobox__currentIndexChanged (int combo_box_index);

private:
    void setSelfUpdating (bool self_updating);
    bool isSelfUpdating ();

    QSharedPointer<SignalVisualisationModel> signal_visualisation_model_;
    QSharedPointer<EventManager> event_manager_;

    QMutex self_updating_mutex_;
    bool self_updating_;

    Ui::EventCreationWidget ui_;
};

}

#endif // EVENT_CREATION_WIDGET_H
