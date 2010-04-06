#include "event_time_selection_dialog.h"
#include "file_handling/event_manager.h"

#include <QListWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

namespace BioSig_
{

//-----------------------------------------------------------------------------
EventTimeSelectionDialog::EventTimeSelectionDialog (std::map<uint16, QString>
                                                    const& shown_event_types,
                                                    std::map<uint32, QString>
                                                    const& shown_channels,
                                                    EventManager const& event_manager)
    : shown_event_types_ (shown_event_types),
      shown_channels_ (shown_channels),
      event_manager_ (event_manager)
{
    QVBoxLayout* top_layout = new QVBoxLayout(this);
    top_layout->setMargin(10);
    top_layout->setSpacing(10);

    channel_list_widget_ = new QListWidget(this);
    channel_list_widget_
        ->setSelectionMode(QAbstractItemView::MultiSelection);
    top_layout->addWidget(channel_list_widget_);
    for (std::map<uint32, QString>::const_iterator channels_it = shown_channels_.begin();
         channels_it != shown_channels_.end();
         ++channels_it)
    {
        channel_list_widget_->addItem(QString("(%1) %2").arg(channels_it->first + 1)
                                      .arg(channels_it->second));
    }

    QVBoxLayout* event_layout = new QVBoxLayout(this);
    event_type_combobox_ = new QComboBox (this);
    event_layout->addWidget (event_type_combobox_);

    for (std::map<uint16, QString>::const_iterator event_it = shown_event_types_.begin();
         event_it != shown_event_types_.end();
         ++event_it)
    {
        event_type_combobox_->addItem(event_it->second);
    }

    QGridLayout* event_type_info_layout = new QGridLayout (this);
    event_type_info_layout->addWidget(new QLabel(tr("Include seconds before events"), this), 1, 1);
    seconds_before_spinbox_ = new QDoubleSpinBox (this);
    seconds_before_spinbox_->setMinimum (0);
    seconds_before_spinbox_->setSingleStep(0.25);
    seconds_before_spinbox_->setValue(0);
    event_type_info_layout->addWidget(seconds_before_spinbox_, 1, 2);

    event_type_info_layout->addWidget(new QLabel(tr("Length (in seconds)"), this), 2, 1);
    length_spinbox_ = new QDoubleSpinBox (this);
    length_spinbox_->setMinimum (0);
    length_spinbox_->setSingleStep(0.25);
    length_spinbox_->setValue(0);
    event_type_info_layout->addWidget(length_spinbox_, 2, 2);

    event_type_average_time_label_ = new QLabel (this);
    event_type_average_time_label_->setWordWrap (true);
    event_type_amount_label_ = new QLabel (this);

    event_type_info_layout->addWidget (new QLabel(tr("Amount of Events"), this), 3, 1);
    event_type_info_layout->addWidget (event_type_amount_label_, 3, 2);
    event_type_info_layout->addWidget (new QLabel(tr("Average Event Duration"), this), 4, 1);
    event_type_info_layout->addWidget (event_type_average_time_label_, 4, 2);

    event_layout->addLayout (event_type_info_layout);

    QGroupBox* event_group_box = new QGroupBox ("Event Type", this);
    event_group_box->setLayout (event_layout);
    top_layout->addWidget(event_group_box);

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setMargin(0);
    button_layout->addStretch(1);
    ok_button_ = new QPushButton(tr("OK"), this);
    button_layout->addWidget(ok_button_);
    ok_button_->setFocus();
    cancel_button_ = new QPushButton(tr("Cancel"), this);
    button_layout->addWidget(cancel_button_);
    button_layout->addStretch(1);
    top_layout->addLayout(button_layout);

    connect(ok_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(reject()));
    connect(event_type_combobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedEventTypeChanged(int)));
    selectedEventTypeChanged (0);
}

//-----------------------------------------------------------------------------
std::vector<uint32> EventTimeSelectionDialog::getSelectedChannels () const
{
    std::vector<uint32> channels;

    for (std::map<uint32, QString>::const_iterator channels_it = shown_channels_.begin();
         channels_it != shown_channels_.end();
         ++channels_it)
    {
        if (channel_list_widget_->isItemSelected(channel_list_widget_->item(channels_it->first)))
            channels.push_back (channels_it->first);
    }

    return channels;
}

//-----------------------------------------------------------------------------
uint16 EventTimeSelectionDialog::getSelectedEventType () const
{    
    std::map<uint16, QString>::const_iterator event_it = shown_event_types_.begin();
    for (int i = 0; i < event_type_combobox_->currentIndex(); i++)
        ++event_it;
    return event_it->first;
}

//-----------------------------------------------------------------------------
float EventTimeSelectionDialog::getSecondsBeforeEvent () const
{
    return seconds_before_spinbox_->value();
}

//-----------------------------------------------------------------------------
float EventTimeSelectionDialog::getLengthInSeconds () const
{
    return length_spinbox_->value();
}

//-----------------------------------------------------------------------------
void EventTimeSelectionDialog::selectedEventTypeChanged (int combo_box_index)
{
    std::map<uint16, QString>::const_iterator event_type_it = shown_event_types_.begin();
    if (event_type_it == shown_event_types_.end())
        return;

    for (int i = 0; i < combo_box_index; i++)
        ++event_type_it;

    QList<EventID> event_ids = event_manager_.getEvents(event_type_it->first);

    float32 shortest_duration = 100000.0; // FIXMEEE!!!!!!!!!!!!!!!!!! max float32!!
    float32 longest_duration = 0;
    float32 average_duration = 0;

    for (QList<EventID>::ConstIterator id_iter = event_ids.begin();
         id_iter != event_ids.end();
         ++id_iter)
    {
        float32 dur = event_manager_.getEvent(*id_iter)->getDurationInSec();
        if (dur > longest_duration)
            longest_duration = dur;
        if (dur < shortest_duration)
            shortest_duration = dur;
        average_duration += dur;
    }
    average_duration /= event_ids.size();

    event_type_amount_label_->setText (QString::number(event_ids.size()));

    QString text_for_time_statistic = QString::number(average_duration) + QString("s");
    if (average_duration != longest_duration || average_duration != shortest_duration)
    {
        text_for_time_statistic += QString (" (max: ") + QString::number(longest_duration) +
                                   QString ("s; min: ") + QString::number(shortest_duration)+
                                   QString ("s)");
    }

    event_type_average_time_label_->setText (text_for_time_statistic);
    length_spinbox_->setValue(average_duration);
}



}
