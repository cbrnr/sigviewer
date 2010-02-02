#include "event_info_widget.h"

#include "signal_browser_model_4.h"
#include "change_type_undo_command.h"
#include "new_event_undo_command.h"
#include "../command_stack.h"

#include <QGridLayout>
#include <QPushButton>
#include <QAction>

#include <cmath>
#include <iostream>

namespace BioSig_
{

//-------------------------------------------------------------------
EventInfoWidget::EventInfoWidget(QWidget* parent,
                                         QSharedPointer<SignalBrowserModel> signal_browser_model,
                                         std::map<uint16, QString> const& shown_event_types)
    : QWidget (parent),
      signal_browser_model_ (signal_browser_model),
      event_creation_type_ (1),
      shown_event_types_ (shown_event_types)
{
    layout_ = new QVBoxLayout(this);
    layout_->setSpacing (50);

    QGridLayout* creation_info_layout = new QGridLayout (this);
    event_creation_type_combobox_ = new QComboBox (this);
    event_type_combobox_ = new QComboBox (this);
    for (std::map<uint16, QString>::const_iterator event_it = shown_event_types_.begin();
         event_it != shown_event_types_.end();
         ++event_it)
    {
        event_creation_type_combobox_->addItem (event_it->second, QVariant(event_it->first));
        event_type_combobox_->addItem (event_it->second, QVariant(event_it->first));
    }
    event_type_combobox_->setEnabled (false);
    creation_info_layout->addWidget (new QLabel(tr("Creation Type"), this), 1, 1);
    creation_info_layout->addWidget (event_creation_type_combobox_, 1, 2);
    connect(event_creation_type_combobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selfChangedCreationType(int)));
    connect(event_type_combobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selfChangedType(int)));
    layout_->addLayout (creation_info_layout);

    insert_event_button_ = new QPushButton (tr("Insert Event"), this);
    insert_event_button_->setEnabled (false);
    layout_->addWidget (insert_event_button_);
    connect (insert_event_button_, SIGNAL(clicked()), this, SLOT(insertEvent()));

    QGridLayout* info_layout = new QGridLayout (this);
    info_layout->setVerticalSpacing(1);
    layout_->addLayout(info_layout);

    id_label_ = new QLabel (this);
    duration_label_ = new QLabel (this);
    start_label_ = new QLabel (this);
    info_layout->addWidget(new QLabel(QString("Type")), 1, 1);
    info_layout->addWidget(event_type_combobox_, 1, 2);
    info_layout->addWidget(new QLabel(QString("ID")), 2, 1);
    info_layout->addWidget(id_label_, 2, 2);
    info_layout->addWidget(new QLabel(QString("Begin")), 3, 1);
    info_layout->addWidget(start_label_, 3, 2);
    info_layout->addWidget(new QLabel(QString("Duration")), 4, 1);
    info_layout->addWidget(duration_label_, 4, 2);

    setLayout(layout_);
}

//-------------------------------------------------------------------
EventInfoWidget::~EventInfoWidget()
{
    // nothing to do here
}

//-------------------------------------------------------------------
void EventInfoWidget::updateSelectedEventInfo (QSharedPointer<SignalEvent> selected_signal_event)
{
    selected_signal_event_ = selected_signal_event;
    if (selected_signal_event_.isNull())
    {
        insert_event_button_->setEnabled (false);
        event_type_combobox_->setEnabled (false);
    }
    else
    {
        insert_event_button_->setEnabled (true);
        event_type_combobox_->setEnabled (true);
        event_type_combobox_->setCurrentIndex (event_type_combobox_->findData (QVariant(selected_signal_event->getType())));
        id_label_->setText(QString::number(selected_signal_event->getId()));
        duration_label_->setText(QString::number(round(selected_signal_event->getDurationInSec()*100)/100) + QString(" s"));
        start_label_->setText(QString::number(round(selected_signal_event->getPositionInSec()*100)/100) + QString(" s"));
    }
}

//-------------------------------------------------------------------
void EventInfoWidget::updateCreationType (uint16 new_creation_type)
{
    event_creation_type_ = new_creation_type;
}

//-------------------------------------------------------------------
void EventInfoWidget::selfChangedCreationType (int combo_box_index)
{
    bool ok;
    event_creation_type_ = event_creation_type_combobox_->itemData(combo_box_index).toUInt(&ok);
    emit eventCreationTypeChanged (event_creation_type_);
}

//-------------------------------------------------------------------
void EventInfoWidget::selfChangedType (int combo_box_index)
{
    bool ok;
    uint16 event_type = event_type_combobox_->itemData(combo_box_index).toUInt(&ok);

    ChangeTypeUndoCommand* change_type_command = new ChangeTypeUndoCommand (selected_signal_event_, event_type);
    connect (change_type_command, SIGNAL(eventChanged(int32)), signal_browser_model_.data(), SLOT(setEventChanged(int32)));
    CommandStack::instance().executeEditCommand(change_type_command);
}


//-------------------------------------------------------------------
void EventInfoWidget::insertEvent ()
{
    bool ok;
    uint16 event_type = event_creation_type_combobox_->itemData(event_creation_type_combobox_->currentIndex()).toUInt(&ok);
    QSharedPointer<SignalEvent> new_event = QSharedPointer<SignalEvent>(new SignalEvent(*selected_signal_event_));
    new_event->setType (event_type);

    NewEventUndoCommand* new_event_command = new NewEventUndoCommand (*signal_browser_model_, new_event);
    CommandStack::instance().executeEditCommand(new_event_command);
}

}
