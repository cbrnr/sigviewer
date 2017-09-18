// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "scale_channel_dialog.h"

#include <QDebug>
#include <QSettings>
#include <QDesktopWidget>

#include <limits>

namespace sigviewer
{

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

    QDesktopWidget dw;
    int x=dw.width()*0.22;
    int y=dw.height()*0.22;
    resize(x,y);

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

    ui_.unitLabelLower->hide();
    ui_.unitLabelUpper->hide();

    QSettings settings;
    if (settings.value("autoScaling", true).toBool())
        ui_.autoButton->setChecked(true);
    else
        ui_.manualButton->setChecked(true);

    last_max_ = settings.value("scaling_upper_value", 75).toDouble();
    last_min_ = settings.value("scaling_lower_value", -75).toDouble();

    ui_.upper_spinbox_->setValue(last_max_);
    ui_.lower_spinbox_->setValue(last_min_);
    ui_.upper_spinbox_->setMinimum(ui_.lower_spinbox_->value());
    ui_.lower_spinbox_->setMaximum(ui_.upper_spinbox_->value());
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
void ScaleChannelDialog::on_manualButton_toggled (bool checked)
{
    ui_.upper_spinbox_->setEnabled(checked);
    ui_.lower_spinbox_->setEnabled(checked);

    if (!checked)
        return;

    ui_.upper_spinbox_->setValue (last_max_);
    ui_.lower_spinbox_->setValue (last_min_);

    QSettings settings;
    settings.setValue("autoScaling", false);
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
    else
    {
        upper_value = last_max_;
        lower_value = last_min_;
    }

    ui_.upper_spinbox_->setValue (upper_value);
    ui_.lower_spinbox_->setValue (lower_value);

    QSettings settings;
    settings.setValue("autoScaling", true);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::storeAccepted ()
{
    if (ui_.manualButton->isChecked())
    {
        last_max_ = ui_.upper_spinbox_->value();
        last_min_ = ui_.lower_spinbox_->value();

        QSettings settings;
        settings.setValue("scaling_upper_value", last_max_);
        settings.setValue("scaling_lower_value", last_min_);
    }
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_upper_spinbox__valueChanged(double arg1)
{
    ui_.lower_spinbox_->setMaximum(arg1);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_lower_spinbox__valueChanged(double arg1)
{
    ui_.upper_spinbox_->setMinimum(arg1);
}

}

