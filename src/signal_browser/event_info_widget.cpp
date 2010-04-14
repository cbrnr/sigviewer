#include "event_info_widget.h"

#include "signal_browser_model_4.h"
#include "change_type_undo_command.h"
#include "new_event_undo_command.h"
#include "resize_event_undo_command.h"
#include "../command_executer.h"
#include "../file_handling/event_manager.h"


#include <QPushButton>
#include <QAction>
#include <QMutexLocker>

#include <cmath>
#include <set>
#include <iostream>

namespace BioSig_
{

//-------------------------------------------------------------------
EventInfoWidget::EventInfoWidget(QWidget* parent,
                                 QSharedPointer<EventManager> event_manager,
                                 QSharedPointer<CommandExecuter> command_executer,
                                 QSharedPointer<SignalBrowserModel> signal_browser_model)
    : QWidget (parent),
      signal_browser_model_ (signal_browser_model),
      event_manager_ (event_manager),
      command_executer_ (command_executer)
{
    this->setWindowTitle (tr("Event Toolbox"));
    layout_ = new QHBoxLayout(this);
    event_type_combobox_ = new QComboBox (this);
    disabled_widgets_if_nothing_selected_.push_back(event_type_combobox_);
    duration_spinbox_ = new QDoubleSpinBox (this);
    duration_spinbox_->setSuffix (tr("s"));
    duration_spinbox_->setMinimum (0);
    duration_spinbox_->setSingleStep (0.1);
    duration_spinbox_->setDecimals (2);
    duration_spinbox_->setKeyboardTracking (false);
    connect (duration_spinbox_, SIGNAL(valueChanged(double)), this, SLOT(selfChangedDuration(double)));
    disabled_widgets_if_nothing_selected_.push_back(duration_spinbox_);
    start_label_ = new QLabel (this);
    disabled_widgets_if_nothing_selected_.push_back(start_label_);

    QLabel* type_label = new QLabel (tr("Type"), this);
    type_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel* start_desc_label = new QLabel (tr("Begin"), this);
    start_desc_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    QLabel* duration_label = new QLabel (tr("Duration"), this);
    duration_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    layout_->addWidget(type_label);
    layout_->addWidget(event_type_combobox_);
    layout_->addWidget(start_desc_label);
    layout_->addWidget(start_label_);
    layout_->addWidget(duration_label);
    layout_->addWidget(duration_spinbox_);

    setLayout(layout_);
    foreach (QWidget* widget, disabled_widgets_if_nothing_selected_)
        widget->setEnabled(false);
}

//-------------------------------------------------------------------
EventInfoWidget::~EventInfoWidget()
{
    // nothing to do here
}

//-------------------------------------------------------------------
void EventInfoWidget::updateSelectedEventInfo (QSharedPointer<SignalEvent const>
                                               selected_signal_event)
{
    setSelfUpdating (true);
    selected_signal_event_ = selected_signal_event;
    if (selected_signal_event_.isNull())
    {
        foreach (QWidget* widget, disabled_widgets_if_nothing_selected_)
            widget->setEnabled (false);
    }
    else
    {
        foreach (QWidget* widget, disabled_widgets_if_nothing_selected_)
            widget->setEnabled (true);
        event_type_combobox_->setCurrentIndex (event_type_combobox_->findData (QVariant(selected_signal_event->getType())));
        duration_spinbox_->setValue(selected_signal_event->getDurationInSec());
        start_label_->setText(QString::number(round(selected_signal_event->getPositionInSec()*100)/100) + QString(" s"));
    }
    setSelfUpdating (false);
}

//-------------------------------------------------------------------
void EventInfoWidget::updateShownEventTypes (std::set<uint16> shown_event_types)
{
    shown_event_types_.clear ();
    event_type_combobox_->disconnect();
    event_type_combobox_->clear();

    for (std::set<uint16>::const_iterator it = shown_event_types.begin();
         it != shown_event_types.end();
         ++it)
    {
        QString event_name = event_manager_->getNameOfEventType (*it);
        shown_event_types_[*it] = event_name;
        event_type_combobox_->addItem (event_name, QVariant(*it));
    }
    connect(event_type_combobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selfChangedType(int)));
}

//-------------------------------------------------------------------
void EventInfoWidget::selfChangedType (int combo_box_index)
{
    if (isSelfUpdating())
        return;

    uint16 event_type = event_type_combobox_->itemData(combo_box_index).toUInt();

    if (event_type != selected_signal_event_->getType())
    {
        ChangeTypeUndoCommand* change_type_command = new ChangeTypeUndoCommand (event_manager_, selected_signal_event_->getId(), event_type);
        command_executer_->executeCommand (change_type_command);
    }
}

//-------------------------------------------------------------------
void EventInfoWidget::selfChangedDuration (double new_duration)
{
    if (selected_signal_event_.isNull())
        return;
    if (isSelfUpdating())
        return;

    int rounding_factor = pow (10, duration_spinbox_->decimals());
    double rounded_old_duration = round (selected_signal_event_->getDurationInSec() * rounding_factor);
    rounded_old_duration /= rounding_factor;
    if (rounded_old_duration == new_duration)
        return;

    int new_duration_in_samples = new_duration * selected_signal_event_->getSampleRate ();
    ResizeEventUndoCommand* resize_command = new ResizeEventUndoCommand (event_manager_, selected_signal_event_->getId(), selected_signal_event_->getPosition(), new_duration_in_samples);
    command_executer_->executeCommand (resize_command);
}

//-------------------------------------------------------------------
void EventInfoWidget::setSelfUpdating (bool self_updating)
{
    QMutexLocker locker (&self_updating_mutex_);
    self_updating_ = self_updating;
}

//-------------------------------------------------------------------
bool EventInfoWidget::isSelfUpdating ()
{
    QMutexLocker locker (&self_updating_mutex_);
    return self_updating_;
}



}
