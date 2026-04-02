// © SigViewer developers
//
// License: GPL-3.0

#include "scale_channel_dialog.h"

#include <QListWidgetItem>
#include <QSettings>
#include <cmath>
#include <limits>

namespace sigviewer {

//-----------------------------------------------------------------------------
// In-process memory: persists across dialog openings, reset when app exits.
static std::set<ChannelID> s_lastSelection;

//-----------------------------------------------------------------------------
ScaleChannelDialog::ScaleChannelDialog(std::set<ChannelID> const& shown_channels,
    ChannelManager const& channel_manager,
    std::set<ChannelID> const& preselected,
    QWidget* parent)
    : QDialog(parent), shown_channels_(shown_channels), channel_manager_(channel_manager) {
    ui_.setupUi(this);
    setWindowTitle(tr("Scale Channels"));

    ui_.upperSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui_.upperSpinBox->setMinimum(-std::numeric_limits<double>::max());
    ui_.lowerSpinBox->setMaximum(std::numeric_limits<double>::max());
    ui_.lowerSpinBox->setMinimum(-std::numeric_limits<double>::max());

    // Determine which channels to pre-check:
    //   - explicit preselected set (e.g. from right-click) → use it as-is
    //   - otherwise restore the last in-process selection (all checked on first open)
    std::set<ChannelID> const& toCheck =
        !preselected.empty() ? preselected
                             : (!s_lastSelection.empty() ? s_lastSelection : shown_channels_);

    for (ChannelID id : shown_channels_) {
        auto* item = new QListWidgetItem(channel_manager_.getChannelLabel(id), ui_.channelList);
        item->setData(Qt::UserRole, id);
        item->setCheckState(toCheck.count(id) ? Qt::Checked : Qt::Unchecked);
    }

    // Restore last-used settings
    QSettings settings;
    int modeIndex = settings.value("ScaleDialog/mode", 0).toInt();
    bool sym = settings.value("ScaleDialog/symmetric", true).toBool();
    double lastUpper = settings.value("ScaleDialog/upperValue", 75.0).toDouble();
    double lastLower = settings.value("ScaleDialog/lowerValue", -75.0).toDouble();

    ui_.upperSpinBox->setValue(lastUpper);
    ui_.lowerSpinBox->setValue(lastLower);
    ui_.symmetricCheckBox->setChecked(sym);

    if (modeIndex == 1)
        ui_.autoSharedRangeButton->setChecked(true);
    else if (modeIndex == 2)
        ui_.manualButton->setChecked(true);
    else
        ui_.autoPerChannelButton->setChecked(true);

    bool isManual = (modeIndex == 2);
    setManualControlsEnabled(isManual);
    setSymmetricEnabled(!isManual);

    connect(this, SIGNAL(accepted()), SLOT(storeAccepted()));
}

//-----------------------------------------------------------------------------
ScaleChannelDialog::ScalingMode ScaleChannelDialog::scalingMode() const {
    if (ui_.autoSharedRangeButton->isChecked()) return AUTO_SHARED_RANGE;
    if (ui_.manualButton->isChecked()) return MANUAL;
    return AUTO_PER_CHANNEL;
}

//-----------------------------------------------------------------------------
bool ScaleChannelDialog::symmetric() const {
    return ui_.symmetricCheckBox->isChecked();
}

//-----------------------------------------------------------------------------
std::set<ChannelID> ScaleChannelDialog::selectedChannels() const {
    std::set<ChannelID> result;
    for (int i = 0; i < ui_.channelList->count(); ++i) {
        QListWidgetItem* item = ui_.channelList->item(i);
        if (item->checkState() == Qt::Checked)
            result.insert(item->data(Qt::UserRole).toInt());
    }
    return result;
}

//-----------------------------------------------------------------------------
float64 ScaleChannelDialog::upperValue() const { return ui_.upperSpinBox->value(); }

//-----------------------------------------------------------------------------
float64 ScaleChannelDialog::lowerValue() const { return ui_.lowerSpinBox->value(); }

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_autoPerChannelButton_toggled(bool checked) {
    if (!checked) return;
    setManualControlsEnabled(false);
    setSymmetricEnabled(true);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_autoSharedRangeButton_toggled(bool checked) {
    if (!checked) return;
    setManualControlsEnabled(false);
    setSymmetricEnabled(true);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_manualButton_toggled(bool checked) {
    if (!checked) return;
    setManualControlsEnabled(true);
    setSymmetricEnabled(false);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_selectAllButton_clicked() {
    for (int i = 0; i < ui_.channelList->count(); ++i)
        ui_.channelList->item(i)->setCheckState(Qt::Checked);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_selectNoneButton_clicked() {
    for (int i = 0; i < ui_.channelList->count(); ++i)
        ui_.channelList->item(i)->setCheckState(Qt::Unchecked);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_upperSpinBox_valueChanged(double value) {
    ui_.lowerSpinBox->setMaximum(value);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::on_lowerSpinBox_valueChanged(double value) {
    ui_.upperSpinBox->setMinimum(value);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::storeAccepted() {
    // Persist channel selection in-process
    s_lastSelection = selectedChannels();

    QSettings settings;
    int modeIndex = (scalingMode() == AUTO_PER_CHANNEL)    ? 0
                    : (scalingMode() == AUTO_SHARED_RANGE) ? 1
                                                           : 2;
    settings.setValue("ScaleDialog/mode", modeIndex);
    settings.setValue("ScaleDialog/symmetric", ui_.symmetricCheckBox->isChecked());
    if (ui_.manualButton->isChecked()) {
        settings.setValue("ScaleDialog/upperValue", ui_.upperSpinBox->value());
        settings.setValue("ScaleDialog/lowerValue", ui_.lowerSpinBox->value());
    }
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::setManualControlsEnabled(bool enabled) {
    ui_.upperLabel->setEnabled(enabled);
    ui_.lowerLabel->setEnabled(enabled);
    ui_.upperSpinBox->setEnabled(enabled);
    ui_.lowerSpinBox->setEnabled(enabled);
}

//-----------------------------------------------------------------------------
void ScaleChannelDialog::setSymmetricEnabled(bool enabled) {
    ui_.symmetricCheckBox->setEnabled(enabled);
}

}  // namespace sigviewer
