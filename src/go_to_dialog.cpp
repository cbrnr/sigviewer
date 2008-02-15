// go_to_dialog.cpp

#include "go_to_dialog.h"
#include "base/basic_header.h"

#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSettings>

namespace BioSig_
{

// constructor
GoToDialog::GoToDialog(QPointer<BasicHeader> header, QWidget* parent)
 : QDialog(parent),
   basic_header_(header)
{
    setWindowTitle(tr("Go To"));
    QVBoxLayout* top_layout = new QVBoxLayout(this);
    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    QGridLayout* input_layout = new QGridLayout(top_layout);
    int32 duration = (int32)(basic_header_->getNumberRecords() * 
                             basic_header_->getRecordDuration());
    sec_spinbox_ = new QSpinBox(this);
	sec_spinbox_->setMinimum(0);    
    sec_spinbox_->setMaximum(duration);
    sec_spinbox_->setSingleStep(10);
    channel_combobox_ = new QComboBox(this);
    channel_combobox_->addItem(tr("None"));
    input_layout->addWidget(new QLabel(tr("Second"), this), 1, 1);
    input_layout->addWidget(new QLabel(tr("Channel"), this), 2, 1);
    input_layout->addWidget(sec_spinbox_, 1, 2);
    input_layout->addWidget(channel_combobox_, 2, 2);
    QHBoxLayout* button_layout = new QHBoxLayout(top_layout);
    button_layout->setMargin(10);
    button_layout->addStretch(1);
    ok_button_ = new QPushButton(tr("OK"), this);
    button_layout->addWidget(ok_button_);
    cancel_button_ = new QPushButton(tr("Cancel"), this);
    button_layout->addWidget(cancel_button_);
    button_layout->addStretch(1);

    top_layout->activate();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(ok_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(reject()));
}

// load settings
void GoToDialog::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("GoToDialog");
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    sec_spinbox_->setValue(settings.value("second", 0).toInt());
    channel_combobox_->setCurrentIndex(settings.value("channel", -1).toInt() + 1);
    settings.endGroup();
}

// save settings
void GoToDialog::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("GoToDialog");
    settings.setValue("pos", pos());
    settings.setValue("second", sec_spinbox_->value());
    settings.setValue("channel", channel_combobox_->currentIndex() - 1);
    settings.endGroup();
}

// get second
int32 GoToDialog::getSecond()
{
    return sec_spinbox_->value();
}

// get channel index
int32 GoToDialog::getChannelIndex()
{
    return channel_combobox_->currentIndex() - 1;
}

//set channel shown
void GoToDialog::setChannelShown(uint32 channel_nr)
{
    const SignalChannel& channel = basic_header_->getChannel(channel_nr);
    channel_combobox_->addItem(QString("(%1) %2").arg(channel_nr + 1)
                                        .arg(channel.getLabel()));
}

} //namespace BioSig_
