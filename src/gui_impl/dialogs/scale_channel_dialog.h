#ifndef SCALEC_HANNEL_DIALOG_H
#define SCALEC_HANNEL_DIALOG_H

#include "../../base/sigviewer_user_types.h"
#include "../../file_handling/channel_manager.h"

#include "ui_scale_channel_dialog.h"

#include <QDialog>

namespace BioSig_
{

class ScaleChannelDialog : public QDialog
{
Q_OBJECT
public:
    explicit ScaleChannelDialog (ChannelID preselected_channel,
                                 std::set<ChannelID> const& shown_channels,
                                 QSharedPointer<ChannelManager const> channel_manager,
                                 QWidget *parent = 0);


    ChannelID channel () const;

    float upperValue () const;
    float lowerValue () const;
signals:

private slots:
    void on_channels__currentIndexChanged (int index);
private:
    ChannelID selected_channel_;
    std::set<ChannelID> const shown_channels_;
    QSharedPointer<ChannelManager const> channel_manager_;

    Ui::ScaleChannelDialog ui_;
};
}
#endif // SCALEC_HANNEL_DIALOG_H
