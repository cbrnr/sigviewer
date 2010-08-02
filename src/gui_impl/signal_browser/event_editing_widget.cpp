#include "event_editing_widget.h"
#include "gui/gui_action_factory.h"
#include "editing_commands/change_type_undo_command.h"
#include "editing_commands/resize_event_undo_command.h"
#include "base/math_utils.h"

#include <QMutexLocker>
#include <QDebug>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
EventEditingWidget::EventEditingWidget (QSharedPointer<EventManager> event_manager,
                                        QSharedPointer<CommandExecuter> command_executer,
                                        QWidget *parent) :
    QWidget(parent),
    event_manager_ (event_manager),
    command_executer_ (command_executer),
    self_updating_ (false)
{
    ui_.setupUi (this);

    ui_.previous_button_->setDefaultAction (GuiActionFactory::getInstance()->getQAction("Goto and Select Previous Event"));
    ui_.next_button_->setDefaultAction (GuiActionFactory::getInstance()->getQAction("Goto and Select Next Event"));
    ui_.fit_button_->setDefaultAction (GuiActionFactory::getInstance()->getQAction("Fit View to Selected Event"));

    unsigned precision = MathUtils_::sampleRateToDecimalPrecision(event_manager_->getSampleRate());
    ui_.begin_spinbox_->setDecimals (precision);
    ui_.begin_spinbox_->setMaximum (static_cast<double>(event_manager_->getMaxEventPosition()) / event_manager_->getSampleRate ());
    ui_.duration_spinbox_->setDecimals (precision);
}

//-----------------------------------------------------------------------------
void EventEditingWidget::updateShownEventTypes (std::set<EventType> const& shown_event_types)
{
    setSelfUpdating (true);

    ui_.type_combobox_->clear ();
    foreach (EventType type, shown_event_types)
        ui_.type_combobox_->addItem (event_manager_->getNameOfEventType (type),
                                     QVariant (type));

    setSelfUpdating (false);
}

//-----------------------------------------------------------------------------
void EventEditingWidget::updateSelectedEventInfo (QSharedPointer<SignalEvent const>
                                                  selected_signal_event)
{
    setSelfUpdating (true);
    selected_signal_event_ = selected_signal_event;
    if (selected_signal_event_.isNull())
        ui_.selection_frame_->setDisabled (true);
    else
    {
        ui_.selection_frame_->setEnabled (true);
        ui_.type_combobox_->setCurrentIndex (ui_.type_combobox_->findData (QVariant(selected_signal_event_->getType())));
        ui_.duration_spinbox_->setValue (selected_signal_event_->getDurationInSec ());
        ui_.duration_spinbox_->setMaximum (static_cast<double>(event_manager_->getMaxEventPosition () -
                                                               selected_signal_event->getPosition()) /
                                           event_manager_->getSampleRate ());
        ui_.begin_spinbox_->setValue (selected_signal_event_->getPositionInSec ());
        bool has_no_previous_event = event_manager_->getPreviousEventOfSameType (selected_signal_event->getId ())
                                     == UNDEFINED_EVENT_ID;
        bool has_no_next_event = event_manager_->getNextEventOfSameType (selected_signal_event->getId ())
                                 == UNDEFINED_EVENT_ID;
        ui_.next_button_->setDisabled (has_no_next_event);
        ui_.previous_button_->setDisabled (has_no_previous_event);
    }
    setSelfUpdating (false);
}

//-------------------------------------------------------------------
void EventEditingWidget::on_type_combobox__currentIndexChanged (int combo_box_index)
{
    if (isSelfUpdating() || selected_signal_event_.isNull ())
        return;

    EventType event_type = ui_.type_combobox_->itemData(combo_box_index).toUInt();

    if (event_type != selected_signal_event_->getType())
    {
        ChangeTypeUndoCommand* change_type_command = new ChangeTypeUndoCommand (event_manager_, selected_signal_event_->getId(), event_type);
        command_executer_->executeCommand (change_type_command);
    }
}

//-------------------------------------------------------------------
void EventEditingWidget::on_begin_spinbox__editingFinished ()
{
    if (isSelfUpdating () || selected_signal_event_.isNull())
        return;

    unsigned new_begin = ui_.begin_spinbox_->value()
                         * event_manager_->getSampleRate ();

    ResizeEventUndoCommand* resize_command =
            new ResizeEventUndoCommand (event_manager_,
                                        selected_signal_event_->getId(),
                                        new_begin,
                                        selected_signal_event_->getDuration());
    command_executer_->executeCommand (resize_command);
}

//-------------------------------------------------------------------
void EventEditingWidget::on_duration_spinbox__editingFinished ()
{
    if (isSelfUpdating () || selected_signal_event_.isNull())
        return;

    unsigned new_duration = ui_.duration_spinbox_->value()
                            * event_manager_->getSampleRate ();

    ResizeEventUndoCommand* resize_command =
            new ResizeEventUndoCommand (event_manager_,
                                        selected_signal_event_->getId(),
                                        selected_signal_event_->getPosition(),
                                        new_duration);
    command_executer_->executeCommand (resize_command);
}


//-------------------------------------------------------------------
void EventEditingWidget::setSelfUpdating (bool self_updating)
{
    QMutexLocker locker (&self_updating_mutex_);
    self_updating_ = self_updating;
}

//-------------------------------------------------------------------
bool EventEditingWidget::isSelfUpdating ()
{
    QMutexLocker locker (&self_updating_mutex_);
    return self_updating_;
}

}
