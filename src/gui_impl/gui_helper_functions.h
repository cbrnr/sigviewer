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
