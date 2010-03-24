#include "event_info_widget.h"

#include "signal_browser_model_4.h"
#include "change_type_undo_command.h"
#include "new_event_undo_command.h"
#include "resize_event_undo_command.h"
#include "../command_stack.h"


//#include <QGridLayout>
#include <QPushButton>
#include <QAction>
//#include <QGroupBox>
#include <QMutexLocker>

#include <cmath>
#include <set>
#include <iostream>

namespace BioSig_
{

//-------------------------------------------------------------------
EventInfoWidget::EventInfoWidget(QWidget* parent,
                                 QSharedPointer<SignalBrowserModel> signal_browser_model)
    : QWidget (parent),
      signal_browser_model_ (signal_browser_model)//,
//      event_creation_type_ (1)
{
    this->setWindowTitle (tr("Event Toolbox"));
    layout_ = new QHBoxLayout(this);
//    layout_->setSpacing (50);

//    QGroupBox* settings_group_box = new QGroupBox (tr("Settings"), this);
//    settings_group_box->setFlat(false);
//    QVBoxLayout* creation_info_layout = new QVBoxLayout (settings_group_box);
//    event_creation_type_combobox_ = new QComboBox (this);
    event_type_combobox_ = new QComboBox (this);
    disabled_widgets_if_nothing_selected_.push_back(event_type_combobox_);
//    creation_info_layout->addWidget (new QLabel(tr("Creation Type"), this));
//    creation_info_layout->addWidget (event_creation_type_combobox_);
//    layout_->addWidget (settings_group_box);
//
//    QGroupBox* editing_tools_group_box = new QGroupBox (tr("Editing Tools"), this);
//    QVBoxLayout* editing_tools_layout = new QVBoxLayout (editing_tools_group_box);
//    insert_event_button_ = new QPushButton (tr("Insert Event"), editing_tools_group_box);
//    disabled_widgets_if_nothing_selected_.push_back(insert_event_button_);
//    editing_tools_layout->addWidget(insert_event_button_);
//    layout_->addWidget (editing_tools_group_box);
//    connect (insert_event_button_, SIGNAL(clicked()), this, SLOT(insertEvent()));

//    QGroupBox* selected_event_group_box = new QGroupBox (tr("Selected Event"), this);
//    QGridLayout* info_layout = new QGridLayout (this);
//    info_layout->setVerticalSpacing(1);

//    id_label_ = new QLabel (this);//selected_event_group_box);
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
//    layout_->addWidget(new QLabel(QString("ID"), this));
//    layout_->addWidget(id_label_);
    layout_->addWidget(start_desc_label);
    layout_->addWidget(start_label_);
    layout_->addWidget(duration_label);
    layout_->addWidget(duration_spinbox_);
    //selected_event_group_box->setLayout (info_layout);
    //layout_->addWidget (selected_event_group_box);

//    hovered_info_label_ = new QLabel ();
//    hovered_info_label_->setWordWrap (true);
//    QGroupBox* hovered_event_group_box = new QGroupBox (tr("Hovered Events"), this);
//    QVBoxLayout* hovered_layout = new QVBoxLayout (hovered_event_group_box);
//    hovered_layout->addWidget (hovered_info_label_);
//    hovered_event_group_box->setLayout(hovered_layout);
//    layout_->addWidget (hovered_event_group_box);

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
void EventInfoWidget::updateSelectedEventInfo (QSharedPointer<SignalEvent> selected_signal_event)
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
//        id_label_->setText(QString::number(selected_signal_event->getId()));
        duration_spinbox_->setValue(selected_signal_event->getDurationInSec());
        start_label_->setText(QString::number(round(selected_signal_event->getPositionInSec()*100)/100) + QString(" s"));
    }
    setSelfUpdating (false);
}

//-------------------------------------------------------------------
void EventInfoWidget::updateCreationType (uint16 new_creation_type)
{
//    event_creation_type_ = new_creation_type;
}

//-------------------------------------------------------------------
void EventInfoWidget::updateShownEventTypes (std::set<uint16> shown_event_types)
{
    shown_event_types_.clear ();
//    event_creation_type_combobox_->disconnect();
    event_type_combobox_->disconnect();
//    event_creation_type_combobox_->clear();
    event_type_combobox_->clear();

    for (std::set<uint16>::const_iterator it = shown_event_types.begin();
         it != shown_event_types.end();
         ++it)
    {
        QString event_name = signal_browser_model_->getEventName(*it);
        shown_event_types_[*it] = event_name;
//        event_creation_type_combobox_->addItem (event_name, QVariant(*it));
        event_type_combobox_->addItem (event_name, QVariant(*it));
    }
//    connect(event_creation_type_combobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selfChangedCreationType(int)));
    connect(event_type_combobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selfChangedType(int)));
}

//-------------------------------------------------------------------
void EventInfoWidget::addHoveredEvent (QSharedPointer<SignalEvent const> event)
{
    hovered_events_.insert (event->getId(), event);
    updateHoveredInfoLabel ();
}

//-------------------------------------------------------------------
void EventInfoWidget::removeHoveredEvent (QSharedPointer<SignalEvent const> event)
{
    hovered_events_.remove(event->getId ());
    updateHoveredInfoLabel ();
}

//-------------------------------------------------------------------
void EventInfoWidget::selfChangedCreationType (int combo_box_index)
{
//    uint16 new_type = event_creation_type_combobox_->itemData(combo_box_index).toUInt();
//    if (event_creation_type_ != new_type)
//    {
//        event_creation_type_ = new_type;
//        if (!isSelfUpdating())
//            emit eventCreationTypeChanged (event_creation_type_);
//    }
}

//-------------------------------------------------------------------
void EventInfoWidget::selfChangedType (int combo_box_index)
{
    if (isSelfUpdating())
        return;

    uint16 event_type = event_type_combobox_->itemData(combo_box_index).toUInt();

    if (event_type != selected_signal_event_->getType())
    {
        ChangeTypeUndoCommand* change_type_command = new ChangeTypeUndoCommand (selected_signal_event_, event_type);
        connect (change_type_command, SIGNAL(eventChanged(int32)), signal_browser_model_.data(), SLOT(setEventChanged(int32)));
        CommandStack::instance().executeEditCommand(change_type_command);
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
    ResizeEventUndoCommand* resize_command = new ResizeEventUndoCommand (*(signal_browser_model_.data()), selected_signal_event_, selected_signal_event_->getPosition(), new_duration_in_samples);
    CommandStack::instance().executeEditCommand (resize_command);
}

//-------------------------------------------------------------------
void EventInfoWidget::insertEvent ()
{
//    bool ok;
//    uint16 event_type = event_creation_type_combobox_->itemData(event_creation_type_combobox_->currentIndex()).toUInt(&ok);
//    if (!ok)
//        return;
//    QSharedPointer<SignalEvent> new_event = QSharedPointer<SignalEvent>(new SignalEvent(*selected_signal_event_));
//    new_event->setType (event_type);
//
//    NewEventUndoCommand* new_event_command = new NewEventUndoCommand (*signal_browser_model_, new_event);
//    CommandStack::instance().executeEditCommand(new_event_command);
}

//-------------------------------------------------------------------
void EventInfoWidget::updateHoveredInfoLabel ()
{
    QMap<int32, QSharedPointer<SignalEvent const> >::const_iterator iterator = hovered_events_.begin ();
    QString text;
    while (iterator != hovered_events_.end())
    {
        text.append (signal_browser_model_->getEventName(iterator.value()->getType()));
        ++iterator;
    }
    // hovered_info_label_->setText (text);
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
