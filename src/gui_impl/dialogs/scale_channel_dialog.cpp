#include "scale_channel_dialog.h"

#include <QDebug>

#include <limits>

namespace SigViewer_
{

double ScaleChannelDialog::last_min_ = 0;
double ScaleChannelDialog::last_max_ = 0;

//-----------------------------------------------------------------------------
ScaleChannelDialog::ScaleChannelDialog (ChannelID preselected_channel,
                                        std::set<ChannelID> const& shown_channels,
                                        ChannelManager const& channel_manager,
                                        QWidget *parent) :
    QDialog(parent),
    selected_channel_ (preselected_channel),
    shown_channels_ (shown_channels),
    channel_manager_ (channel_manager)
{
    ui_.setupUi (this);

    if (selected_channel_ == UNDEFINED_CHANNEL)
        setWindowTitle (tr("Scale All Channels"));
    else
        setWindowTitle (tr("Scale Channel ").append(channel_manager_.getChannelLabel(preselected_channel)));

    ui_.upper_spinbox_->setMaximum (std::numeric_limits<double>::max());
    ui_.upper_spinbox_->setMinimum (-std::numeric_limits<double>::max());
    ui_.lower_spinbox_->setMaximum (std::numeric_limits<double>::max());
    ui_.lower_spinbox_->setMinimum (-std::numeric_limits<double>::max());
    QString unit_string = channel_manager_.getChannelYUnitString (selected_channel_);
    if (unit_string.size())
        ui_.unitLabelLower->setText (QString ("(").append (unit_string).append (")"));
    ui_.unitLabelUpper->setText (ui_.unitLabelLower->text());
    if (selected_channel_ != UNDEFINED_CHANNEL)
    {
        ui_.upper_spinbox_->setValue (channel_manager_.getMaxValue (selected_channel_));
        ui_.lower_spinbox_->setValue (channel_manager_.getMinValue (selected_channel_));
    }
    connect (this, SIGNAL(accepted()), SLOT(storeAccepted()));
}

//-----------------------------------------------------------------------------
bool ScaleChannelDialog::autoScaling () const
{
    return ui_.autoButton->isChecked ();
}

//-----------------------------------------------------------------------------
bool ScaleChannelDialog::physAutoScaling () const
{
    return false;//ui_.physButton->isChecked();
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
void ScaleChannelDialog::on_fixedButton_toggled (bool checked)
{
    if (!checked)
        return;
    double upper_value = last_max_;
    double lower_value = last_min_;

    ui_.upper_spinbox_->setValue (upper_value);
    ui_.lower_spinbox_->setValue (lower_value);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_autoButton_toggled (bool checked)
{
    if (!checked)
        return;
    double upper_value = 0;
    double lower_value = 0;
    if (selected_channel_ != UNDEFINED_CHANNEL)
    {
        upper_value = channel_manager_.getMaxValue (selected_channel_);
        lower_value = channel_manager_.getMinValue (selected_channel_);
    }
    ui_.upper_spinbox_->setValue (upper_value);
    ui_.lower_spinbox_->setValue (lower_value);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::storeAccepted ()
{
    if (ui_.fixedButton->isChecked())
    {
        last_max_ = ui_.upper_spinbox_->value();
        last_min_ = ui_.lower_spinbox_->value();
    }
}


}
