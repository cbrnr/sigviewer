// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


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
