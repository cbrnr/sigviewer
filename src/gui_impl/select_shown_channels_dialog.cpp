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


#include "select_shown_channels_dialog.h"

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>

namespace sigviewer
{

//-----------------------------------------------------------------------------
SelectShownChannelsDialog::SelectShownChannelsDialog (QString const& dialog_title,
                                                      std::set<ChannelID> const& channels,
                                                      ChannelManager const& channel_manager)
    : channels_ (channels)
{
    setWindowTitle (dialog_title);
    QVBoxLayout* top_layout = new QVBoxLayout(this);
    top_layout->setMargin(10);
    top_layout->setSpacing(10);
    channel_list_widget_ = new QListWidget(this);
    channel_list_widget_->setSelectionMode(QAbstractItemView::ExtendedSelection);

    for (std::set<ChannelID>::const_iterator channel_iter = channels_.begin();
         channel_iter != channels_.end ();
         ++channel_iter)
    {
        QString label = channel_manager.getChannelLabel(*channel_iter);
        channel_list_widget_->addItem(QString("(%1) %2").arg (*channel_iter + 1)
                                                .arg(label));
    }

    top_layout->addWidget (channel_list_widget_);
    QHBoxLayout* button_layout = new QHBoxLayout(this);
    button_layout->setMargin(10);
    button_layout->addStretch(1);
    ok_button_ = new QPushButton(tr("OK"), this);
    button_layout->addWidget(ok_button_);
    cancel_button_ = new QPushButton(tr("Cancel"), this);
    button_layout->addWidget(cancel_button_);
    button_layout->addStretch(1);
    top_layout->addLayout(button_layout);
    top_layout->activate();
    connect(ok_button_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button_, SIGNAL(clicked()), this, SLOT(reject()));
}


//-----------------------------------------------------------------------------
std::set<ChannelID> SelectShownChannelsDialog::getSelectedChannels () const
{
    QList<QListWidgetItem*> find_list;
    std::set<ChannelID> selected_channels;
    for (std::set<ChannelID>::const_iterator channel_iter = channels_.begin();
         channel_iter != channels_.end ();
         ++channel_iter)
    {
        find_list = channel_list_widget_
            ->findItems(QString("(%1)").arg(*channel_iter + 1),
                        Qt::MatchStartsWith);
        if (find_list.size() > 0 &&
            channel_list_widget_->isItemSelected(find_list[0]))
            selected_channels.insert (*channel_iter);
    }
    return selected_channels;
}

} //namespace SigViewer_
