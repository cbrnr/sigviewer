#ifndef SCALEC_HANNEL_DIALOG_H
#define SCALEC_HANNEL_DIALOG_H

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"

#include "ui_scale_channel_dialog.h"

#include <QDialog>

namespace SigViewer_
{

class ScaleChannelDialog : public QDialog
{
Q_OBJECT
public:
    explicit ScaleChannelDialog (ChannelID preselected_channel,
                                 std::set<ChannelID> const& shown_channels,
                                 ChannelManager const& channel_manager,
                                 QWidget *parent = 0);

    bool autoScaling () const;
    bool physAutoScaling () const;
    float upperValue () const;
    float lowerValue () const;
signals:

private slots:
    void on_fixedButton_toggled (bool checked);
    void on_autoButton_toggled (bool checked);
    void storeAccepted ();
private:
    ChannelID selected_channel_;
    std::set<ChannelID> const shown_channels_;
    ChannelManager const& channel_manager_;

    Ui::ScaleChannelDialog ui_;

    static double last_min_;
    static double last_max_;
};
}
#endif // SCALEC_HANNEL_DIALOG_H
