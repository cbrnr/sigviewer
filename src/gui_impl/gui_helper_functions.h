#ifndef GUI_HELPER_FUNCTIONS_H
#define GUI_HELPER_FUNCTIONS_H

#include "../base/user_types.h"
#include "../file_handling/channel_manager.h"
#include "../gui/signal_visualisation_model.h"
#include "../gui/color_manager.h"

#include <set>

namespace BioSig_
{

namespace GuiHelper
{

//-----------------------------------------------------------------------------
QSharedPointer<SignalEvent const> getSelectedEvent (QSharedPointer<SignalVisualisationModel>
                                                    signal_visualisation_model);

//-----------------------------------------------------------------------------
EventID getSelectedEventID (QSharedPointer<SignalVisualisationModel>
                            signal_visualisation_model);

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
std::set<ChannelID> selectChannels (QSharedPointer<ChannelManager const> channel_manager,
                                    QSharedPointer<ColorManager> color_manager,
                                    QString const& file_name = "",
                                    QSharedPointer<SignalVisualisationModel> vis_model
                                    = QSharedPointer<SignalVisualisationModel>(0));

//-----------------------------------------------------------------------------
QString getFilePathFromSaveAsDialog (QString const& path,
                                     QString const& extensions,
                                     QString const& file_type_description);


}

}

#endif // GUI_HELPER_FUNCTIONS_H
