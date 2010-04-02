// channel_selection_dialog.cpp

#include "channel_selection_dialog.h"
#include "file_handling/basic_header.h"

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QSettings>  
#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>

namespace BioSig_
{

// constructor
ChannelSelectionDialog::ChannelSelectionDialog(QPointer<BasicHeader> header,
                                               QWidget* parent)
 : QDialog(parent),
   basic_header_(header)
{
    setWindowTitle(tr("Channel Selection"));
    QVBoxLayout* top_layout = new QVBoxLayout(this);
    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    channel_list_widget_ = new QListWidget(this);
    channel_list_widget_
        ->setSelectionMode(QAbstractItemView::MultiSelection);
    top_layout->addWidget(channel_list_widget_);

//    QHBoxLayout* tmpLayout= new QHBoxLayout;
//    top_layout->addItem(tmpLayout);
//    tmpLayout->addWidget(new QLabel("Whole Subsampling:", this));
//    whole_buffer_combobox_ = new QComboBox(this);
//    tmpLayout->addWidget(whole_buffer_combobox_);
//    whole_buffer_combobox_->addItem("None");
//    whole_buffer_combobox_->addItem("1:16");
//    whole_buffer_combobox_->addItem("1:64");
//    whole_buffer_combobox_->addItem("1:256");
//    whole_buffer_combobox_->addItem("1:1024");
//    whole_buffer_combobox_->addItem("1:4096");
//
//    init_range_search_checkbox_ = new QCheckBox("Initial Min-Max Search", this);
//    top_layout->addWidget(init_range_search_checkbox_);
//
//    tmpLayout= new QHBoxLayout;
//    top_layout->addItem(tmpLayout);
//    tmpLayout->addWidget(new QLabel("Default Range:", this));
//    min_spinbox_ = new QDoubleSpinBox(this);
//    min_spinbox_->setRange(-1000000, 1000000);
//    tmpLayout->addWidget(min_spinbox_);
//    tmpLayout->addWidget(new QLabel("-", this));
//    max_spinbox_ = new QDoubleSpinBox(this);
//    max_spinbox_->setRange(-1000000, 1000000);
//    tmpLayout->addWidget(max_spinbox_);

    QHBoxLayout* button_layout = new QHBoxLayout(this);
    QHBoxLayout* select_buttons_layout = new QHBoxLayout(this);
    top_layout->addLayout(select_buttons_layout);
    top_layout->addLayout(button_layout);
    select_buttons_layout->setMargin(0);
    select_buttons_layout->addStretch(1);

    unselect_all_button_ = new QPushButton(tr("Unselect All"), this);
    select_buttons_layout->addWidget(unselect_all_button_);
    select_all_button_ = new QPushButton(tr("Select All"), this);
    select_buttons_layout->addWidget(select_all_button_);
    select_buttons_layout->addStretch(1);

    button_layout->setMargin(0);
    button_layout->addStretch(1);
    ok_button_ = new QPushButton(tr("OK"), this);
    button_layout->addWidget(ok_button_);
    ok_button_->setFocus();
    cancel_button_ = new QPushButton(tr("Cancel"), this);
    button_layout->addWidget(cancel_button_);
    button_layout->addStretch(1);
    buildChannelList();
    top_layout->activate();

    connect(ok_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(reject()));
    connect(unselect_all_button_, SIGNAL(clicked()), this, SLOT(unselectAll()));
    connect(select_all_button_, SIGNAL(clicked()), this, SLOT(selectAll()));
//    connect(max_spinbox_, SIGNAL(valueChanged(double)),
//            this, SLOT(maxChanged(double)));
//    connect(min_spinbox_, SIGNAL(valueChanged(double)),
//            this, SLOT(minChanged(double)));
//    connect(init_range_search_checkbox_, SIGNAL(toggled(bool)),
//            this, SLOT(rangeSearchChanged(bool)));

//    init_range_search_checkbox_->setChecked(true);
//    min_spinbox_->setValue(-100);
//    max_spinbox_->setValue(100);
//    whole_buffer_combobox_->setCurrentIndex(3);
}

// load settings
void ChannelSelectionDialog::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("ChannelSelectionDialog");
    resize(settings.value("size", QSize(250, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
//    init_range_search_checkbox_->setChecked(settings.value("initminmaxsearch").toBool());
//    whole_buffer_combobox_->setCurrentIndex(settings.value("wholesubsampling")
//                                                         .toInt());
//    min_spinbox_->setValue(settings.value("rangemin").toDouble());
//    max_spinbox_->setValue(settings.value("rangemax").toDouble());
//    if (min_spinbox_->value() == max_spinbox_->value())
//    {
//        min_spinbox_->setValue(-100);
//        max_spinbox_->setValue(100);
//    }
    settings.endGroup();
}

// save settings
void ChannelSelectionDialog::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("ChannelSelectionDialog");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
//    settings.setValue("initminmaxsearch", init_range_search_checkbox_->isChecked());
//    settings.setValue("wholesubsampling", whole_buffer_combobox_
//                                              ->currentIndex());
//    settings.setValue("rangemin", min_spinbox_->value());
//    settings.setValue("rangemax", max_spinbox_->value());
    settings.endGroup();
}

// build channel list
void ChannelSelectionDialog::buildChannelList()
{
    for (uint32 channel_nr = 0;
         channel_nr < basic_header_->getNumberChannels();
         channel_nr++)
    {
        const SignalChannel& channel = basic_header_->getChannel(channel_nr);
        channel_list_widget_->addItem(QString("(%1) %2").arg(channel_nr + 1)
                                            .arg(channel.getLabel()));
    }
}

// is selected
bool ChannelSelectionDialog::isSelected(uint32 channel_nr)
{
    return channel_list_widget_->isItemSelected(
                                    channel_list_widget_->item(channel_nr));
}

// set selected
void ChannelSelectionDialog::setSelected(uint32 channel_nr, bool selected)
{
    channel_list_widget_->setItemSelected(
                                        channel_list_widget_->item(channel_nr),
                                        selected);
}

//bool ChannelSelectionDialog::isInitRangeSearch() const
//{
//    return init_range_search_checkbox_->isChecked();
//}

//uint32 ChannelSelectionDialog::wholeSubsampling() const
//{
//    switch(whole_buffer_combobox_->currentIndex())
//    {
//    case 0:
//        return 0;
//    case 1:
//        return 4;
//    case 2:
//        return 6;
//    case 3:
//        return 8;
//    case 4:
//        return 10;
//    case 5:
//        return 12;
//    default:
//        return 0;
//    }
//}

//float32 ChannelSelectionDialog::rangeMin() const
//{
//    return (float32)min_spinbox_->value();
//}
//float32 ChannelSelectionDialog::rangeMax() const
//{
//    return (float32)max_spinbox_->value();
//}

void ChannelSelectionDialog::unselectAll ()
{
    for (int32 channel_number = 0; channel_number < channel_list_widget_->count(); ++channel_number)
        channel_list_widget_->item(channel_number)->setSelected(false);
}

void ChannelSelectionDialog::selectAll ()
{
    for (int32 channel_number = 0; channel_number < channel_list_widget_->count(); ++channel_number)
        channel_list_widget_->item(channel_number)->setSelected(true);
}

//void ChannelSelectionDialog::maxChanged(double value)
//{
//    min_spinbox_->setRange(-1000000, value);
//}
//
//void ChannelSelectionDialog::minChanged(double value)
//{
//    max_spinbox_->setRange(value, 1000000);
//}
//
//void ChannelSelectionDialog::rangeSearchChanged(bool checked)
//{
//    min_spinbox_->setDisabled(checked);
//    max_spinbox_->setDisabled(checked);
//}

} //namespace BioSig_
