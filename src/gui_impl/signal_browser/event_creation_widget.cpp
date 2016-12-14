#include "event_creation_widget.h"

#include <QMutexLocker>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
EventCreationWidget::EventCreationWidget (QSharedPointer<SignalVisualisationModel> signal_visualisation_model,
                                          QSharedPointer<EventManager> event_manager,
                                          QWidget* parent) :
    QWidget(parent),
    signal_visualisation_model_ (signal_visualisation_model),
    event_manager_ (event_manager),
    self_updating_ (false)
{
    ui_.setupUi (this);
}

//-----------------------------------------------------------------------------
void EventCreationWidget::updateShownEventTypes (std::set<EventType> const& shown_event_types)
{
    setSelfUpdating (true);
    ui_.type_combobox_->clear ();
    int current_index = 0;
    foreach (EventType type, shown_event_types)
    {
        ui_.type_combobox_->addItem (event_manager_->getNameOfEventType (type),
                                     QVariant (type));
        if (type == signal_visualisation_model_->getActualEventCreationType())
            current_index = ui_.type_combobox_->count() - 1;
    }

    ui_.type_combobox_->setCurrentIndex (current_index);
    setSelfUpdating (false);
}

//-----------------------------------------------------------------------------
void EventCreationWidget::on_type_combobox__currentIndexChanged (int combo_box_index)
{
    if (isSelfUpdating ())
        return;

    EventType event_type = ui_.type_combobox_->itemData(combo_box_index).toUInt();
    signal_visualisation_model_->setActualEventCreationType (event_type);
}



//-------------------------------------------------------------------
void EventCreationWidget::setSelfUpdating (bool self_updating)
{
    QMutexLocker locker (&self_updating_mutex_);
    self_updating_ = self_updating;
}

//-------------------------------------------------------------------
bool EventCreationWidget::isSelfUpdating ()
{
    QMutexLocker locker (&self_updating_mutex_);
    return self_updating_;
}




}
