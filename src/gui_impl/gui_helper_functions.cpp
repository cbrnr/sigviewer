#include "gui_helper_functions.h"

#include "channel_selection_dialog.h"


namespace BioSig_
{

namespace GuiHelper
{

//-----------------------------------------------------------------------------
std::set<ChannelID> selectChannels (QSharedPointer<ChannelManager> channel_manager,
                                    QString const& file_name,
                                    QSharedPointer<SignalVisualisationModel> vis_model)
{
    ChannelSelectionDialog channel_dialog (channel_manager, file_name);
    std::set<ChannelID> pre_selected_channels;
    if (!vis_model.isNull())
        pre_selected_channels = vis_model->getShownChannels ();

    bool empty_selection = pre_selected_channels.size () == 0;

    for (ChannelID channel_id = 0;
         channel_id < channel_manager->getNumberChannels();
         channel_id++)
    {
        bool show_channel = empty_selection ||
                            (pre_selected_channels.count(channel_id) > 0);

        if (show_channel)
            channel_dialog.setSelected (channel_id, show_channel);
    }

    channel_dialog.loadSettings();
    channel_dialog.exec();
    channel_dialog.saveSettings();

    if (channel_dialog.result() == QDialog::Rejected)
        return pre_selected_channels;

    std::set<ChannelID> selected_channels;
    for (ChannelID channel_id = 0;
         channel_id < channel_manager->getNumberChannels();
         channel_id++)
    {
        if (channel_dialog.isSelected (channel_id))
            selected_channels.insert (channel_id);
    }
    return selected_channels;
}

}

}
