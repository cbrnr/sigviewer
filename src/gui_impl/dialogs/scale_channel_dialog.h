// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


#ifndef SCALEC_HANNEL_DIALOG_H
#define SCALEC_HANNEL_DIALOG_H

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"

#include "ui_scale_channel_dialog.h"

#include <QDialog>

namespace sigviewer
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
