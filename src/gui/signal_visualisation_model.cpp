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


#include "signal_visualisation_model.h"

#include <QDebug>

namespace sigviewer
{

//-----------------------------------------------------------------------------
SignalVisualisationModel::SignalVisualisationModel (std::set<EventType> const& shown_types,
                                                    ChannelManager const& channel_manager)
    : signal_view_settings_ (new SignalViewSettings (channel_manager)),
      //pixel_per_sample_ (1),
      mode_ (MODE_HAND),
      event_creation_type_ (1),
      shown_event_types_ (shown_types),
      scale_mode_ (MIN_TO_MAX),
      selected_channel_ (UNDEFINED_CHANNEL),
      info_widget_ (0)
{
    // nothing to do here
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setMode (SignalVisualisationMode mode)
{
    mode_ = mode;
    emit modeChanged (mode_);
    modeChangedImpl (mode_);
}

//-----------------------------------------------------------------------------
SignalVisualisationMode SignalVisualisationModel::getMode () const
{
    return mode_;
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setInfoWidget (QWidget* info_widget)
{
    info_widget_ = info_widget;
}

//-------------------------------------------------------------------------
ChannelID SignalVisualisationModel::getSelectedChannel () const
{
    return selected_channel_;
}

//-------------------------------------------------------------------------
void SignalVisualisationModel::selectChannel (ChannelID channel)
{
    selected_channel_ = channel;
}


//-----------------------------------------------------------------------------
QList<EventID> SignalVisualisationModel::getSelectedEvents () const
{
    return QList<EventID> ();
}


//-----------------------------------------------------------------------------
std::set<EventType> SignalVisualisationModel::getShownEventTypes () const
{
    return shown_event_types_;
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setShownEventTypes (std::set<EventType> const& event_types)
{
    shown_event_types_ = event_types;
    emit shownEventTypesChanged (shown_event_types_);
    shownEventTypesChangedImpl ();
}

//-----------------------------------------------------------------------------
EventType SignalVisualisationModel::getActualEventCreationType () const
{
    return event_creation_type_;
}

//-------------------------------------------------------------------------
void SignalVisualisationModel::setAutoScaleMode (ScaleMode scale_mode)
{
    scale_mode_ = scale_mode;
}

//-------------------------------------------------------------------------
ScaleMode SignalVisualisationModel::getAutoScaleMode () const
{
    return scale_mode_;
}

//-----------------------------------------------------------------------------
void SignalVisualisationModel::setActualEventCreationType (EventType type)
{
    event_creation_type_ = type;
}

//-----------------------------------------------------------------------------
QWidget* SignalVisualisationModel::infoWidget ()
{
    return info_widget_;
}

}
