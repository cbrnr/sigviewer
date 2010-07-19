#include "scale_channel_dialog.h"

#include <QDebug>

#include <limits>

namespace BioSig_
{

//-----------------------------------------------------------------------------
ScaleChannelDialog::ScaleChannelDialog (ChannelID preselected_channel,
                                        std::set<ChannelID> const& shown_channels,
                                        QSharedPointer<ChannelManager const> channel_manager,
                                        QWidget *parent) :
    QDialog(parent),
    selected_channel_ (preselected_channel),
    shown_channels_ (shown_channels),
    channel_manager_ (channel_manager)
{
    ui_.setupUi (this);

    ui_.upper_spinbox_->setMaximum (std::numeric_limits<double>::max());
    ui_.upper_spinbox_->setMinimum (-std::numeric_limits<double>::max());
    ui_.lower_spinbox_->setMaximum (std::numeric_limits<double>::max());
    ui_.lower_spinbox_->setMinimum (-std::numeric_limits<double>::max());

    ui_.channels_->addItem (tr("All Channels"), UNDEFINED_CHANNEL);
    foreach (ChannelID id, shown_channels)
        ui_.channels_->addItem (channel_manager_->getChannelLabel(id), id);

    int preselected_index = ui_.channels_->findData (preselected_channel, Qt::UserRole, Qt::MatchExactly);
    ui_.channels_->setCurrentIndex (preselected_index);
}

//-----------------------------------------------------------------------------
ChannelID ScaleChannelDialog::channel () const
{
    return ui_.channels_->itemData (ui_.channels_->currentIndex(),
                                    Qt::UserRole).toUInt();
}

//-----------------------------------------------------------------------------
float ScaleChannelDialog::upperValue () const
{
    return ui_.upper_spinbox_->value();
}

//-----------------------------------------------------------------------------
float ScaleChannelDialog::lowerValue () const
{
    return ui_.lower_spinbox_->value();
}


//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_channels__currentIndexChanged (int index)
{
    selected_channel_ = ui_.channels_->itemData(index, Qt::UserRole).toUInt();

    double upper_value;
    double lower_value;
    if (selected_channel_ == UNDEFINED_CHANNEL)
    {
        upper_value = channel_manager_->getMaxValue (shown_channels_);
        lower_value = channel_manager_->getMinValue (shown_channels_);
    }
    else
    {
        upper_value = channel_manager_->getMaxValue (selected_channel_);
        lower_value = channel_manager_->getMinValue (selected_channel_);
    }
    ui_.upper_spinbox_->setValue (upper_value);
    ui_.lower_spinbox_->setValue (lower_value);
    ui_.upper_spinbox_->setSuffix (QString(" ") + channel_manager_->getChannelYUnitString (selected_channel_));
    ui_.lower_spinbox_->setSuffix (QString(" ") + channel_manager_->getChannelYUnitString (selected_channel_));
}


}
