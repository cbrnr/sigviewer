// channel_selection_dialog.cpp

#include "channel_selection_dialog.h"
#include "base/basic_header.h"

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>

namespace BioSig_
{

// constructor
ChannelSelectionDialog::ChannelSelectionDialog(BasicHeader& header,
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
    QHBoxLayout* button_layout = new QHBoxLayout;
    top_layout->addLayout(button_layout);
    button_layout->setMargin(0);
    button_layout->addStretch(1);
    ok_button_ = new QPushButton(tr("OK"), this);
    button_layout->addWidget(ok_button_);
    cancel_button_ = new QPushButton(tr("Cancel"), this);
    button_layout->addWidget(cancel_button_);
    button_layout->addStretch(1);
    buildChannelList();
    top_layout->activate();
    connect(ok_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(reject()));
}

// load settings
void ChannelSelectionDialog::loadSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("ChannelSelectionDialog");
    resize(settings.value("size", QSize(250, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

// save settings
void ChannelSelectionDialog::saveSettings()
{
    QSettings settings("SigViewer");
    settings.beginGroup("ChannelSelectionDialog");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

// build channel list
void ChannelSelectionDialog::buildChannelList()
{
    for (uint32 channel_nr = 0;
         channel_nr < basic_header_.getNumberChannels();
         channel_nr++)
    {
        const SignalChannel& channel = basic_header_.getChannel(channel_nr);
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

} //namespace BioSig_
