// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "event_creation_widget.h"
#include "file_handling_impl/xdf_reader.h"
#include "event_editing_widget.h" //to sync the event type combobox

#include <QMutexLocker>

namespace sigviewer
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
    if (event_manager_->getFileType().startsWith("XDF", Qt::CaseInsensitive))
    {
        ui_.lineEdit->setPlaceholderText("Customize Event Text");
        customized_event_id_ = XDFdata.dictionary.size();
    }
    else //custom event seems doesn't work in files other than XDF
    {
        ui_.lineEdit->hide();
        ui_.pushButton->hide();
    }
}

//-----------------------------------------------------------------------------
int EventCreationWidget::insertNewEventType()
{
    if (ui_.lineEdit->text().compare(""))
    {
        customized_text_ = ui_.lineEdit->text();
        event_manager_->setEventName(customized_event_id_, customized_text_);

        ui_.type_combobox_->removeItem(customized_event_id_);
        ui_.type_combobox_->insertItem(customized_event_id_, customized_text_, QVariant (customized_event_id_));

        ui_.type_combobox_->setCurrentIndex(customized_event_id_);
        customized_event_id_++;
        if (customized_event_id_ >= 254) //Sigviewer has only 255 slots for custom events
            customized_event_id_ = XDFdata.dictionary.size();
    }

    return 0;
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

//-------------------------------------------------------------------
void EventCreationWidget::on_pushButton_clicked()
{
    insertNewEventType();
}

//-------------------------------------------------------------------
void EventCreationWidget::on_lineEdit_returnPressed() //same as push button clicked
{
    insertNewEventType();
}

}
