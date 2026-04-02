// © SigViewer developers
//
// License: GPL-3.0

#ifndef SCALE_CHANNEL_DIALOG_H
#define SCALE_CHANNEL_DIALOG_H

#include <QDialog>
#include <set>

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"
#include "ui_scale_channel_dialog.h"

namespace sigviewer {

class ScaleChannelDialog : public QDialog {
    Q_OBJECT
   public:
    enum ScalingMode { AUTO_PER_CHANNEL, AUTO_SHARED_RANGE, MANUAL };

    explicit ScaleChannelDialog(std::set<ChannelID> const& shown_channels,
        ChannelManager const& channel_manager,
        std::set<ChannelID> const& preselected = {},
        QWidget* parent = nullptr);

    ScalingMode scalingMode() const;
    bool symmetric() const;
    std::set<ChannelID> selectedChannels() const;
    float64 upperValue() const;
    float64 lowerValue() const;

   private slots:
    void on_autoPerChannelButton_toggled(bool checked);
    void on_autoSharedRangeButton_toggled(bool checked);
    void on_manualButton_toggled(bool checked);
    void on_selectAllButton_clicked();
    void on_selectNoneButton_clicked();
    void on_upperSpinBox_valueChanged(double value);
    void on_lowerSpinBox_valueChanged(double value);
    void storeAccepted();

   private:
    void setManualControlsEnabled(bool enabled);
    void setSymmetricEnabled(bool enabled);

    std::set<ChannelID> const shown_channels_;
    ChannelManager const& channel_manager_;

    Ui::ScaleChannelDialog ui_;
};

}  // namespace sigviewer

#endif  // SCALE_CHANNEL_DIALOG_H
