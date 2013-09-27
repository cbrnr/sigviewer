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


#ifndef SELECT_SHOWN_CHANNELS_DIALOG_H
#define SELECT_SHOWN_CHANNELS_DIALOG_H

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"

#include <QDialog>

#include <set>

class QListWidget;
class QPushButton;

namespace SigViewer_
{

class SelectShownChannelsDialog : public QDialog
{
    Q_OBJECT
public:
    SelectShownChannelsDialog (QString const& dialog_title,
                               std::set<ChannelID> const& channels,
                               ChannelManager const& channel_manager);

    std::set<ChannelID> getSelectedChannels () const;

private:
    SelectShownChannelsDialog ();
    SelectShownChannelsDialog (const SelectShownChannelsDialog&);
    const SelectShownChannelsDialog& operator= (const SelectShownChannelsDialog&);

    std::set<ChannelID> channels_;
    QListWidget* channel_list_widget_;
    QPushButton* ok_button_;
    QPushButton* cancel_button_;
};

} // namespace SigViewer_

#endif // SELECT_SHOWN_CHANNELS_DIALOG_H
