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


#ifndef EVENT_TIME_SELECTION_DIALOG_H
#define EVENT_TIME_SELECTION_DIALOG_H

#include "base/sigviewer_user_types.h"
#include "file_handling/event_manager.h"
#include "file_handling/channel_manager.h"
#include "ui_event_time_selection_dialog.h"

#include <QDialog>
#include <QString>
#include <QSharedPointer>

#include <set>

namespace sigviewer
{

class EventTimeSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    //-------------------------------------------------------------------------
    EventTimeSelectionDialog (std::set<EventType> const& shown_event_types,
                              std::set<ChannelID> const& shown_channels,
                              ChannelManager const& channel_manager,
                              QSharedPointer<EventManager const> event_manager);

    //-------------------------------------------------------------------------
    std::set<ChannelID> getSelectedChannels () const;

    //-------------------------------------------------------------------------
    EventType getSelectedEventType () const;

    //-------------------------------------------------------------------------
    float getSecondsBeforeEvent () const;

    //-------------------------------------------------------------------------
    float getLengthInSeconds () const;

private slots:
    //-------------------------------------------------------------------------
    void on_unselect_all_button__clicked ();

    //-------------------------------------------------------------------------
    void on_select_all_button__clicked ();

    //-------------------------------------------------------------------------
    void on_event_combo_box__currentIndexChanged (int combo_box_index);

    //-------------------------------------------------------------------------
    void on_add_before_spinbox__valueChanged (double value);

    //-------------------------------------------------------------------------
    void on_list_widget__itemSelectionChanged ();

    //-------------------------------------------------------------------------
    void on_length_spinbox__valueChanged (double value);

private:
    //-------------------------------------------------------------------------
    void updateOkEnabled ();

    std::set<EventType> shown_event_types_;
    std::set<ChannelID> shown_channels_;
    QSharedPointer<EventManager const> event_manager_;

    double start_before_value_;
    Ui::EventTimeSelectionDialog ui_;
};

}

#endif // EVENT_TIME_SELECTION_DIALOG_H
