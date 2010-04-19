#ifndef GUI_HELPER_FUNCTIONS_H
#define GUI_HELPER_FUNCTIONS_H

#include "../base/user_types.h"
#include "../file_handling/channel_manager.h"
#include "../gui/signal_visualisation_model.h"

#include <set>

namespace BioSig_
{

namespace GuiHelper
{

//-----------------------------------------------------------------------------
std::set<ChannelID> selectChannels (QSharedPointer<ChannelManager> channel_manager,
                                    QString const& file_name = "",
                                    QSharedPointer<SignalVisualisationModel> vis_model
                                    = QSharedPointer<SignalVisualisationModel>(0));

}

}

#endif // GUI_HELPER_FUNCTIONS_H
