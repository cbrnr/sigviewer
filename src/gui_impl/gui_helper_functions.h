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


#ifndef GUI_HELPER_FUNCTIONS_H
#define GUI_HELPER_FUNCTIONS_H

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"
#include "file_handling/basic_header.h"
#include "gui/signal_visualisation_model.h"
#include "gui/color_manager.h"

#include <set>

namespace SigViewer_
{

namespace GuiHelper
{

//-----------------------------------------------------------------------------
void animateProperty (QObject* target, QByteArray const& property_name,
                      QVariant const& start_value, QVariant const& end_value,
                      QObject* call_back_object = 0, char const* call_back_slot = 0);

//-----------------------------------------------------------------------------
QList<QSharedPointer<SignalEvent const> > getSelectedEvents (QSharedPointer<EventView>
                                                             event_view);

//-----------------------------------------------------------------------------
QList<EventID> getSelectedEventIDs (QSharedPointer<EventView>
                                    event_view);

//-----------------------------------------------------------------------------
EventType selectEventType (EventType preselected_type, QSharedPointer<SignalVisualisationModel>
                           signal_visualisation_model);

//-----------------------------------------------------------------------------
std::set<EventType> selectEventTypes (std::set<EventType> const& preselected_type,
                                      QSharedPointer<EventManager const> event_manager,
                                      QSharedPointer<ColorManager> color_manager,
                                      bool enable_color_editing = false);

//-----------------------------------------------------------------------------
ChannelID selectChannel (ChannelID preselected_channel,
                         QSharedPointer<SignalVisualisationModel> signal_visualisation_model);

//-----------------------------------------------------------------------------
std::set<ChannelID> selectShownChannels (ChannelID hide_channel,
                                         QSharedPointer<SignalVisualisationModel> vis_model);

//-----------------------------------------------------------------------------
std::set<ChannelID> selectChannels (ChannelManager const& channel_manager,
                                    QSharedPointer<ColorManager> color_manager,
                                    QSharedPointer<BasicHeader> header,
                                    QSharedPointer<SignalVisualisationModel> vis_model
                                    = QSharedPointer<SignalVisualisationModel>(0));

//-----------------------------------------------------------------------------
QString getFilePathFromSaveAsDialog (QString const& path,
                                     QString const& extensions,
                                     QString const& file_type_description);


}

}

#endif // GUI_HELPER_FUNCTIONS_H
