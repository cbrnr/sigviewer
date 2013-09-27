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


#include "event_time_selection_dialog.h"
#include "file_handling/event_manager.h"

#include <QPushButton>
#include <limits>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
EventTimeSelectionDialog::EventTimeSelectionDialog (std::set<EventType> const& shown_event_types,
                                                    std::set<ChannelID> const& shown_channels,
                                                    ChannelManager const& channel_manager,
                                                    QSharedPointer<EventManager const> event_manager)
    : shown_event_types_ (shown_event_types),
      shown_channels_ (shown_channels),
      event_manager_ (event_manager),
      start_before_value_ (0)
{
    ui_.setupUi (this);

    foreach (ChannelID channel_id, shown_channels)
    {
        QListWidgetItem* item = new QListWidgetItem (channel_manager.getChannelLabel(channel_id), ui_.list_widget_);
        item->setData (Qt::UserRole, channel_id);
    }

    foreach (EventType event_type, shown_event_types_)
    {
        if (event_manager_->getEvents (event_type).count ())
            ui_.event_combo_box_->addItem (event_manager_->getNameOfEventType(event_type), QVariant(event_type));
    }

    ui_.button_box_->button(QDialogButtonBox::Ok)->setEnabled (false);
    updateOkEnabled ();
}

//-----------------------------------------------------------------------------
std::set<ChannelID> EventTimeSelectionDialog::getSelectedChannels () const
{
    std::set<ChannelID> channels;

    foreach (QListWidgetItem* list_item, ui_.list_widget_->selectedItems())
        channels.insert (list_item->data(Qt::UserRole).toInt());

    return channels;
}

//-----------------------------------------------------------------------------
EventType EventTimeSelectionDialog::getSelectedEventType () const
{    
    return ui_.event_combo_box_->itemData(ui_.event_combo_box_->currentIndex()).toUInt();
}

//-----------------------------------------------------------------------------
float EventTimeSelectionDialog::getSecondsBeforeEvent () const
{
    return ui_.add_before_spinbox_->value();
}

//-----------------------------------------------------------------------------
float EventTimeSelectionDialog::getLengthInSeconds () const
{
    return ui_.length_spinbox_->value();
}

//-----------------------------------------------------------------------------
void EventTimeSelectionDialog::on_unselect_all_button__clicked ()
{
    for (int row = 0; row < ui_.list_widget_->count(); ++row)
        ui_.list_widget_->item (row)->setSelected (false);
}

//-----------------------------------------------------------------------------
void EventTimeSelectionDialog::on_select_all_button__clicked ()
{
    for (int row = 0; row < ui_.list_widget_->count(); ++row)
        ui_.list_widget_->item (row)->setSelected (true);
}

//-----------------------------------------------------------------------------
void EventTimeSelectionDialog::on_event_combo_box__currentIndexChanged (int combo_box_index)
{
    QList<EventID> event_ids = event_manager_->getEvents (ui_.event_combo_box_->itemData(combo_box_index).toUInt());

    float32 shortest_duration = std::numeric_limits<float32>::max();
    float32 longest_duration = 0;
    float32 average_duration = 0;

    for (QList<EventID>::ConstIterator id_iter = event_ids.begin();
         id_iter != event_ids.end();
         ++id_iter)
    {
        float32 dur = event_manager_->getEvent(*id_iter)->getDurationInSec();
        if (dur > longest_duration)
            longest_duration = dur;
        if (dur < shortest_duration)
            shortest_duration = dur;
        average_duration += dur;
    }
    average_duration /= event_ids.size();

    ui_.number_of_events_label_->setText (QString::number(event_ids.size()));

    QString text_for_time_statistic = QString::number(average_duration) + QString("s");
    if (average_duration != longest_duration || average_duration != shortest_duration)
    {
        text_for_time_statistic += QString (" (max: ") + QString::number(longest_duration) +
                                   QString ("s; min: ") + QString::number(shortest_duration)+
                                   QString ("s)");
    }

    //event_type_average_time_label_->setText (text_for_time_statistic);
    ui_.length_spinbox_->setValue (average_duration + start_before_value_);
}

//-----------------------------------------------------------------------------
void EventTimeSelectionDialog::on_add_before_spinbox__valueChanged (double value)
{
    ui_.length_spinbox_->setValue (ui_.length_spinbox_->value() - start_before_value_ + value);
    start_before_value_ = value;
}

//-----------------------------------------------------------------------------
void EventTimeSelectionDialog::on_list_widget__itemSelectionChanged ()
{
    updateOkEnabled ();
}

//-------------------------------------------------------------------------
void EventTimeSelectionDialog::on_length_spinbox__valueChanged (double)
{
    updateOkEnabled ();
}

//-------------------------------------------------------------------------
void EventTimeSelectionDialog::updateOkEnabled ()
{
    int num_selected_items = ui_.list_widget_->selectedItems().count();
    bool nothing_selected = (num_selected_items == 0);
    bool all_selected = (num_selected_items == ui_.list_widget_->count());
    bool length_is_zero = (ui_.length_spinbox_->value() == 0);
    bool no_event_selected = (ui_.event_combo_box_->currentIndex() == -1);

    ui_.button_box_->button(QDialogButtonBox::Ok)->setDisabled (nothing_selected
                                                                || length_is_zero
                                                                || no_event_selected);
    ui_.unselect_all_button_->setDisabled (nothing_selected);
    ui_.select_all_button_->setDisabled (all_selected);
}




}
